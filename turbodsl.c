

/*
 *
 *  Turbo DSL Implementaion
 *
 *  Zhicheng Tang   ztang@ti.com
 *
 *  2002 (c) Texas Instruments Inc.
 *
*/

#if 0

/* defines and variables */
#define RFC2684_BRIDGED_HDR_SIZE 10
unsigned char LLC_BRIDGED_HEADER_2684[RFC2684_BRIDGED_HDR_SIZE] =
  {0xAA, 0xAA, 0x03, 0x00, 0x80, 0xC2, 0x00, 0x07, 0x00, 0x00};

#define RFC2684_ROUTED_HDR_SIZE 6
unsigned char LLC_ROUTED_HEADER_2684[6] ={0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00};

unsigned long PPP_LLC_HEADER  = 0xCF03FEFE;

/* struct definition */
enum
{  
  AAL5_ENCAP_PPP_LLC,
  AAL5_ENCAP_PPP_VCMUX,
  AAL5_ENCAP_RFC2684_LLC_BRIDGED,
  AAL5_ENCAP_RFC2684_LLC_ROUTED
};

/* Etherent header */
typedef struct _turbodsl_ether_header 
{
        unsigned char dst_mac_addr[6];
        unsigned char src_mac_addr[6];
        unsigned short ether_type;
} turbodsl_ether_header_t;


/* Ip header define */
typedef struct _turbodsl_ip_header
{

        unsigned short vit;
        unsigned short total_length;
        unsigned short ip_id;
        unsigned char  flag;            /* bit 0 = 0, bit1 = don't fragment, bit2=more frag */
  unsigned char  fragment_offset; /* offset include remaining 5 bits above, which make it 13 bits */
        unsigned char  time_to_live;
        unsigned char  protocol;
        unsigned short checksum;
        unsigned int  src_ip;
        unsigned int  dst_ip;
} turbodsl_ip_header_t;

/* Arp packet define */
typedef struct _turbodsl_arp_header
{
        unsigned short hardware_type;
        unsigned short protocol_type;
        unsigned char  h_len;
        unsigned char  p_len;
        unsigned short operation  ;
        unsigned char  snd_hw_address[6];
        unsigned char  snd_pt_address[4];
        unsigned char  dst_hw_address[6];
        unsigned char  dst_pt_address[4];
} turbodsl_arp_header_t;

#define FIN_FLAG 1
#define SYN_FLAG 1<<1
#define RST_FLAG 1<<2
#define PSH_FLAG 1<<3
#define ACK_FLAG 1<<4
#define URG_FLAG 1<<5

typedef struct _turbodsl_tcp_header
{
        unsigned short src_port;
        unsigned short dst_port;
        unsigned int seq_num;
        unsigned int ack_num;
  unsigned char  offset; /* only bits 4-7 are for offset */
  unsigned char  flags;  /* bits: 0-FIN, 1-SYN, 2-RST, 3-PSH, 4-ACK, 5-URG */
  unsigned short windows; 
        unsigned short checksum;
        unsigned short urgent_ptr;
} turbodsl_tcp_header_t;
 


/***************************************************************************
 * Function: turbodsl_memory_compare
 * Descripation: Memory compare
 ****************************************************************************/
__inline__ int turbodsl_memory_compare(unsigned char *pIn, unsigned char *pOut, unsigned int len)
  {
  int i;
  
  for(i=0;i<(int)len; i++)
    {
    if(pIn[i] != pOut[i])
      return 0;
    }
  return 1;
  }

/***************************************************************************
 * Function: turbodsl_check_aal5_encap_type
 * Descripation: Determine AAL5 Encapsulation type
 * Input:
 *        unsigned char *pData, AAL5 Packet buffer pointer
 ****************************************************************************/
__inline__ static int turbodsl_check_aal5_encap_type(unsigned char *pData)
  {
  
  if(turbodsl_memory_compare(pData, LLC_BRIDGED_HEADER_2684, 6))
    return AAL5_ENCAP_RFC2684_LLC_BRIDGED;
  if(turbodsl_memory_compare(pData, LLC_ROUTED_HEADER_2684, 6))
    return AAL5_ENCAP_RFC2684_LLC_ROUTED;
  if(turbodsl_memory_compare(pData, (unsigned char *)&PPP_LLC_HEADER, sizeof(PPP_LLC_HEADER)))
    return AAL5_ENCAP_PPP_LLC;
 
  return AAL5_ENCAP_PPP_VCMUX;
  }
  
/***************************************************************************
 * Function: turbodsl_check_priority_type
 * Descripation: Determine AAL5 Encapsulation type
 * Input:
 *        unsigned char *pData, AAL5 Packet buffer pointer.
 *        short vpi, VPI.
 *        int vci,  VCI
 ****************************************************************************/
int turbodsl_check_priority_type(unsigned char *pData)
  {
/*  int encap;*/
  unsigned char *pP;
  unsigned short etherType;
  turbodsl_ip_header_t *pIp;
  turbodsl_tcp_header_t *pTcp;
/*  unsigned short ip_length;*/
  
  dprintf(2, "turbodsl_check_priority_type ==>\n");

  /*** Viren: Eliminated local VAriable */
 /* encap = turbodsl_check_aal5_encap_type(pData); */
  pP = pData;

  switch(turbodsl_check_aal5_encap_type(pData))
    {
    case AAL5_ENCAP_RFC2684_LLC_BRIDGED:
      pP += RFC2684_BRIDGED_HDR_SIZE; //skip off aal5 encap
      pP += 12;               //skip of mac address
      etherType = *(unsigned short *)pP;
      if((etherType != 0x6488) && (etherType != 0x0008))
        {
        //Not an IP packet
        return 1;
        }

      pP +=2;                 //skip ether type
      if(etherType == 0x6488)
        {
        pP += 6;
        }
      break;
    case AAL5_ENCAP_RFC2684_LLC_ROUTED:
      pP += RFC2684_ROUTED_HDR_SIZE; //skip of encap
      pP += 2; //skip ether type
      break;
    case AAL5_ENCAP_PPP_LLC:
      pP += sizeof(PPP_LLC_HEADER);
      if((*pP == 0xff) && (*(pP+1) == 0x03)) //ppp hdlc header
        pP += 2;
      break;
    case AAL5_ENCAP_PPP_VCMUX:
      if((*pP == 0xff) && (*(pP+1) == 0x03)) //ppp hdlc header
        pP += 2;
      break;
    default:
      return 1;
    }

  pIp = (turbodsl_ip_header_t *)pP;
  if(pIp->vit != 0x0045)
    {
    //Not a IP packet
    return 1;
    }

  if(pIp->protocol != 0x06) 
    {
    //not tcp packet
    return 1;
    }
  
  pTcp = (turbodsl_tcp_header_t *)(pP + sizeof(turbodsl_ip_header_t));

  /*** Viren: not required ***/
  /*ip_length = ((pIp->total_length>>8) + (pIp->total_length<<8));*/
  
  if((pTcp->flags & ACK_FLAG) && (((pIp->total_length>>8) + (pIp->total_length<<8)) <=40))
    return 0;

  return 1;
  }

#endif














  
