#ifdef USER_MODE
#include <string.h>
#endif
#include "defs.h"
#include "PHY/defs.h"

//#define DEBUG_CH 
int lte_dl_mbsfn_channel_estimation(PHY_VARS_UE *phy_vars_ue,
				    u8 eNB_id,
					u8 eNB_offset,
				    int subframe,
				    unsigned char l)  {
  


  int pilot[600] __attribute__((aligned(16)));
  unsigned char aarx,aa,m;
  unsigned short k;
  unsigned int rb,pilot_cnt;
  short ic[3],*pil,*rxF,*ch,*ch0,*ch1,*ch11,*chp,*ch_prev;
  int ch_offset,symbol_offset;
  int i,c,p;
  //  unsigned int n;
  //  int i;
  u16 Nid_cell_mbsfn = phy_vars_ue->lte_frame_parms.Nid_cell_mbsfn;

  int **dl_ch_estimates=phy_vars_ue->lte_ue_common_vars.dl_ch_estimates[0];
  int **rxdataF=phy_vars_ue->lte_ue_common_vars.rxdataF;

  ch_offset     = (l*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size));
  symbol_offset = phy_vars_ue->lte_frame_parms.ofdm_symbol_size*l;
  
 // m=phy_vars_ue->lte_frame_parms.N_RB_DL*6;
/*  
  if ((l==2) || (l==10)) 
      k = 0;//m<<1;
    else if (l==6)
      k = 1;//+(m<<1);
    else {
      msg("lte_dl_mbsfn: l %d -> ERROR\n",l);
      return(-1);
    }
*/
  for (aarx=0;aarx<phy_vars_ue->lte_frame_parms.nb_antennas_rx;aarx++) {
    // generate pilot
    if ((l==2)||(l==6)||(l==10)) {
      lte_dl_mbsfn_rx(phy_vars_ue,
			   &pilot[0],
			   subframe,
			   l>>2); 
			   } // if symbol==2, return 0 else if symbol = 6, return 1, else if symbol=10 return 2
      
      
      
      pil   = (short *)&pilot[0];
      rxF   = (short *)&rxdataF[aarx][((symbol_offset+phy_vars_ue->lte_frame_parms.first_carrier_offset)<<1)]; 
      ch = (short *)&dl_ch_estimates[aarx][ch_offset];
	       
      //    if (eNb_id==0)
      memset(ch,0,4*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size));
      //***********************************************************************      
      if ((phy_vars_ue->lte_frame_parms.N_RB_DL==6)  || 
	  (phy_vars_ue->lte_frame_parms.N_RB_DL==50) || 
	  (phy_vars_ue->lte_frame_parms.N_RB_DL==100)) {
	
	// Interpolation  and extrapolation;	  
	
}	 
//*********************************************************************	  
      else if (phy_vars_ue->lte_frame_parms.N_RB_DL==25) {
	//printf("Channel estimation\n");
	  //------------------------ loop over first 12 RBs------------------------
	  if (l==6) {
	   // ________________________First half of RBs____________________
		ch+=2;
	    rxF+=4;
	    for (rb=0;rb<12;rb++) {
	      // ------------------------1st pilot------------------------

	      ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	      ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
		  ch[2] = ch[0]>>1;
	      ch[3] = ch[1]>>1;
		    printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
	       if (rb>0) {
		  ch[-2] += ch[2];
		  ch[-1] += ch[3];
	       }
	      else
		   {
		  ch[-2]= ch[0]; 
		  ch[-1]= ch[1];  
		 // ch[-2]= (ch[0]>>1)*3- ch[4];
		 // ch[-1]= (ch[1]>>1)*3- ch[5];  
		   }
	      // ------------------------2nd pilot------------------------
	      ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	      ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
		  ch[6] = ch[4]>>1;
	      ch[7] = ch[5]>>1;
	      ch[2] += ch[6];
	      ch[3] += ch[7];
	        printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);	  
	      // ------------------------3rd pilot------------------------
	      ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	      ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	      ch[10] = ch[8]>>1;
	      ch[11] = ch[9]>>1;
	      ch[6] += ch[10];
	      ch[7] += ch[11];
	       printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]); 
	      // ------------------------4th pilot------------------------
	      ch[12] = (short)(((int)pil[6]*rxF[24] - (int)pil[7]*rxF[25])>>15);
	      ch[13] = (short)(((int)pil[6]*rxF[25] + (int)pil[7]*rxF[24])>>15);
	      ch[14] = ch[12]>>1;
	      ch[15] = ch[13]>>1;
	      ch[10] += ch[14];
	      ch[11] += ch[15];
	        printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[6],pil[7],rxF[24],rxF[25],ch[12],ch[13]);
	      // ------------------------5th pilot------------------------
	      ch[16] = (short)(((int)pil[8]*rxF[32] - (int)pil[9]*rxF[33])>>15);
	      ch[17] = (short)(((int)pil[8]*rxF[33] + (int)pil[9]*rxF[32])>>15);
	      ch[18] = ch[16]>>1;
	      ch[19] = ch[17]>>1;
	      ch[14] += ch[18];
	      ch[15] += ch[19];
	      printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[32],rxF[33],ch[16],ch[17]);
	      // ------------------------6th pilot------------------------
	      ch[20] = (short)(((int)pil[10]*rxF[40] - (int)pil[11]*rxF[41])>>15);
	      ch[21] = (short)(((int)pil[10]*rxF[41] + (int)pil[11]*rxF[40])>>15);
	      ch[22] = ch[20]>>1;
	      ch[23] = ch[21]>>1;
	      ch[18] += ch[22];
	      ch[19] += ch[23];
	      printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[10],pil[11],rxF[40],rxF[41],ch[20],ch[21]);
		  printf("ch11 (%d,%d)\n",ch[22],ch[23]);
	      pil+=12;
	      ch+=24;
	      rxF+=48;
	    }
		// Middle RB
        // ________________________First half of RB___________________	  
	    // ------------------------1st pilot---------------------------
		chp = ch-24;
	    ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	    ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	    ch[2] = ch[0]>>1;
	    ch[3] = ch[1]>>1;
	
	    chp[22] += ch[2];
	    chp[23] += ch[3];
	     printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
	    //printf("interp0: chp[0] (%d,%d) ch23 (%d,%d)\n",chp[22],chp[23],ch[2],ch[3]);
	    // ------------------------2nd pilot------------------------
	    ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	    ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	    ch[6] = ch[4]>>1;
	    ch[7] = ch[5]>>1;
	    ch[2] += ch[6];
	    ch[3] += ch[7];
	    printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);	
		printf("interp1: ch23 (%d,%d) \n",ch[2],ch[3]);	  
	    // ------------------------3rd pilot------------------------
	    ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	    ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	    ch[10] = ch[8]>>1;
	    ch[11] = ch[9]>>1;
		//ch[10] = (ch[8])/3;
	    //ch[11] = (ch[9])/3;
	    ch[6] += ch[10];
	    ch[7] += ch[11];
        printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]);
	    // printf("Second half\n");
		 
	    // ________________________Second half of RB____________________  

	    rxF   = (short *)&rxdataF[aarx][((symbol_offset+2)<<1)]; 

	    // 4th pilot
	    ch[12] = (short)(((int)pil[6]*rxF[0] - (int)pil[7]*rxF[1])>>15);
	    ch[13] = (short)(((int)pil[6]*rxF[1] + (int)pil[7]*rxF[0])>>15);
	    ch[14] = ch[12]>>1;
	    ch[15] = ch[13]>>1;
	    ch[10] = (ch[8]/3)+(ch[12]<<1)/3;
	    ch[11] = (ch[9]/3)+(ch[13]<<1)/3;
	    printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[6],pil[7],rxF[0],rxF[1],ch[12],ch[13]);
	    // ------------------------5th pilot------------------------
	    ch[16] = (short)(((int)pil[8]*rxF[8] - (int)pil[9]*rxF[9])>>15);
	    ch[17] = (short)(((int)pil[8]*rxF[9] + (int)pil[9]*rxF[8])>>15);
	    ch[18] = ch[16]>>1;
	    ch[19] = ch[17]>>1;
	    ch[14] += ch[18];
	    ch[15] += ch[19];
	    printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[8],rxF[9],ch[16],ch[17]);
	    // ------------------------6th pilot------------------------
	    ch[20] = (short)(((int)pil[10]*rxF[16] - (int)pil[11]*rxF[17])>>15);
	    ch[21] = (short)(((int)pil[10]*rxF[17] + (int)pil[11]*rxF[16])>>15);
	    ch[22] = ch[20]>>1;
	    ch[23] = ch[21]>>1;
	    ch[18] += ch[22];
	    ch[19] += ch[23];
        printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[10],pil[11],rxF[16],rxF[17],ch[20],ch[21]);
		pil+=12;
	    ch+=24;
	    rxF+=24;	
	    // ________________________Second half of RBs____________________
	    for (rb=0;rb<11;rb++) {
	      // ------------------------1st pilot---------------------------
	      ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	      ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	      ch[2] = ch[0]>>1;
	      ch[3] = ch[1]>>1;
		   printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
	      //if (rb>0) {
		ch[-2] += ch[2];
		ch[-1] += ch[1];
	      //}
	      // ------------------------2nd pilot------------------------
	      ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	      ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	      ch[6] = ch[4]>>1;
	      ch[7] = ch[5]>>1;
	      ch[2] += ch[6];
	      ch[3] += ch[7];
	      printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);	  
	      // ------------------------3rd pilot------------------------
	      ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	      ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	      ch[10] = ch[8]>>1;
	      ch[11] = ch[9]>>1;
	      ch[6] += ch[10];
	      ch[7] += ch[11];
	      printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]); 
	      // ------------------------4th pilot------------------------
	      ch[12] = (short)(((int)pil[6]*rxF[24] - (int)pil[7]*rxF[25])>>15);
	      ch[13] = (short)(((int)pil[6]*rxF[25] + (int)pil[7]*rxF[24])>>15);
	      ch[14] = ch[12]>>1;
	      ch[15] = ch[13]>>1;
	      ch[10] += ch[14];
	      ch[11] += ch[15];
	      printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[6],pil[7],rxF[24],rxF[25],ch[12],ch[13]);
	      // ------------------------5th pilot------------------------
	      ch[16] = (short)(((int)pil[8]*rxF[32] - (int)pil[9]*rxF[33])>>15);
	      ch[17] = (short)(((int)pil[8]*rxF[33] + (int)pil[9]*rxF[32])>>15);
	      ch[18] = ch[16]>>1;
	      ch[19] = ch[17]>>1;
	      ch[14] += ch[18];
	      ch[15] += ch[19];
	      printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[32],rxF[33],ch[16],ch[17]);
	      // ------------------------6th pilot------------------------
	      ch[20] = (short)(((int)pil[10]*rxF[40] - (int)pil[11]*rxF[41])>>15);
	      ch[21] = (short)(((int)pil[10]*rxF[41] + (int)pil[11]*rxF[40])>>15);
	      ch[22] = ch[20]>>1;
	      ch[23] = ch[21]>>1;
	      ch[18] += ch[22];
	      ch[19] += ch[23];
	      printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[10],pil[11],rxF[40],rxF[41],ch[20],ch[21]);
	      pil+=12;
	      ch+=24;
	      rxF+=48;
	    }
// ------------------------Last PRB ---------------------------
		 // ------------------------1st pilot---------------------------
	      ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	      ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	      ch[2] = ch[0]>>1;
	      ch[3] = ch[1]>>1;		   
		  ch[-2] += ch[2];
		  ch[-1] += ch[3];		 
	        printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
		  // ------------------------2nd pilot------------------------
	      ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	      ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	      ch[6] = ch[4]>>1;
	      ch[7] = ch[5]>>1;
	      ch[2] += ch[6];
	      ch[3] += ch[7];
	      printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);  
	       // ------------------------3rd pilot------------------------
	      ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	      ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	      ch[10] = ch[8]>>1;
	      ch[11] = ch[9]>>1;
	      ch[6] += ch[10];
	      ch[7] += ch[11];
	      printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]);
	      // ------------------------4th pilot------------------------
	      ch[12] = (short)(((int)pil[6]*rxF[24] - (int)pil[7]*rxF[25])>>15);
	      ch[13] = (short)(((int)pil[6]*rxF[25] + (int)pil[7]*rxF[24])>>15);
	      ch[14] = ch[12]>>1;
	      ch[15] = ch[13]>>1;
	      ch[10] += ch[14];
	      ch[11] += ch[15];
	      printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[6],pil[7],rxF[24],rxF[25],ch[12],ch[13]);
	       // ------------------------5th pilot------------------------
	      ch[16] = (short)(((int)pil[8]*rxF[32] - (int)pil[9]*rxF[33])>>15);
	      ch[17] = (short)(((int)pil[8]*rxF[33] + (int)pil[9]*rxF[32])>>15);
	      ch[18] = ch[16]>>1;
	      ch[19] = ch[17]>>1;
	      ch[14] += ch[18];
	      ch[15] += ch[19];
	      printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[32],rxF[33],ch[16],ch[17]);
	      // ------------------------6th pilot------------------------
	      ch[20] = (short)(((int)pil[10]*rxF[40] - (int)pil[11]*rxF[41])>>15);
	      ch[21] = (short)(((int)pil[10]*rxF[41] + (int)pil[11]*rxF[40])>>15);
	      ch[18] += ch[20]>>1;
	      ch[19] += ch[21]>>1;		  
		     printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[10],pil[11],rxF[40],rxF[41],ch[20],ch[21]);	    
	  }     
//**********************************************************************
	  // for l=2 and l=10
	  if (l!=6) {
	    // extrapolate last channel estimate
	    for (rb=0;rb<12;rb++) {
	      // ------------------------1st pilot---------------------------
	      ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	      ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	      ch[2] = ch[0]>>1;
	      ch[3] = ch[1]>>1;	      
		  ch[-2] += ch[2];
		  ch[-1] += ch[3];		 
		   printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
	      // ------------------------2nd pilot------------------------
	      ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	      ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	      ch[6] = ch[4]>>1;
	      ch[7] = ch[5]>>1;
	      ch[2] += ch[6];
	      ch[3] += ch[7];
	  	  printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);
	      // ------------------------3rd pilot------------------------
	      ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	      ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	      ch[10] = ch[8]>>1;
	      ch[11] = ch[9]>>1;
	      ch[6] += ch[10];
	      ch[7] += ch[11];
	  	  printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]);

	      // ------------------------4th pilot------------------------
	      ch[12] = (short)(((int)pil[6]*rxF[24] - (int)pil[7]*rxF[25])>>15);
	      ch[13] = (short)(((int)pil[6]*rxF[25] + (int)pil[7]*rxF[24])>>15);
	      ch[14] = ch[12]>>1;
	      ch[15] = ch[13]>>1;
	      ch[10] += ch[14];
	      ch[11] += ch[15];
	      printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[6],pil[7],rxF[24],rxF[25],ch[12],ch[13]);
	       // ------------------------5th pilot------------------------
	      ch[16] = (short)(((int)pil[8]*rxF[32] - (int)pil[9]*rxF[33])>>15);
	      ch[17] = (short)(((int)pil[8]*rxF[33] + (int)pil[9]*rxF[32])>>15);
	      ch[18] = ch[16]>>1;
	      ch[19] = ch[17]>>1;
	      ch[14] += ch[18];
	      ch[15] += ch[19];
	      printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[32],rxF[33],ch[16],ch[17]);
	      // ------------------------6th pilot------------------------
	      ch[20] = (short)(((int)pil[10]*rxF[40] - (int)pil[11]*rxF[41])>>15);
	      ch[21] = (short)(((int)pil[10]*rxF[41] + (int)pil[11]*rxF[40])>>15);
	      ch[22] = ch[20]>>1;
	      ch[23] = ch[21]>>1;
	      ch[18] += ch[22];
	      ch[19] += ch[23];
	      printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[10],pil[11],rxF[40],rxF[41],ch[20],ch[21]);
	      pil+=12;
	      ch+=24;
	      rxF+=48;
	    }	
	    // ------------------------middle PRB--------------------------
	    // ------------------------1st pilot---------------------------

	    ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	    ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	    ch[2] = ch[0]>>1;
	    ch[3] = ch[1]>>1;
	    printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
	    ch[-2] += ch[2];
		ch[-1] += ch[3];		 
	    // ------------------------2nd pilot------------------------
	    ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	    ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	    ch[6] = ch[4]>>1;
	    ch[7] = ch[5]>>1;
	    ch[2] += ch[6];
	    ch[3] += ch[7];
        printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);
	    // ------------------------3rd pilot------------------------
	    ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	    ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	    ch[10] = ch[8]>>1;
	    ch[11] = ch[9]>>1;
		ch[6] += ch[10];
	    ch[7] += ch[11];
        printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]);
	    // printf("Second half\n");	
		
	    // ------------------------Second half of RBs---------------------//
	    rxF   = (short *)&rxdataF[aarx][((symbol_offset+1)<<1)]; 
        // ---------------------------------------------------------------//
	    // ------------------------4th pilot------------------------
	    ch[12] = (short)(((int)pil[6]*rxF[0] - (int)pil[7]*rxF[1])>>15);
	    ch[13] = (short)(((int)pil[6]*rxF[1] + (int)pil[7]*rxF[0])>>15);
	    ch[14] = ch[12]>>1;
	    ch[15] = ch[13]>>1;
	  	ch[10] = (ch[12]/3)+(ch[8]<<1)/3;
	    ch[11] = (ch[13]/3)+(ch[9]<<1)/3;
        printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[6],pil[7],rxF[0],rxF[1],ch[12],ch[13]);
	     // ------------------------5th pilot------------------------
	    ch[16] = (short)(((int)pil[8]*rxF[8] - (int)pil[9]*rxF[9])>>15);
	    ch[17] = (short)(((int)pil[8]*rxF[9] + (int)pil[9]*rxF[8])>>15);
	    ch[18] = ch[16]>>1;
	    ch[19] = ch[17]>>1;
	    ch[14] += ch[18];
	    ch[15] += ch[19];
	    printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[4],pil[5],rxF[8],rxF[9],ch[8],ch[17]);
	    // ------------------------6th pilot------------------------
	    ch[20] = (short)(((int)pil[10]*rxF[16] - (int)pil[11]*rxF[17])>>15);
	    ch[21] = (short)(((int)pil[10]*rxF[17] + (int)pil[11]*rxF[16])>>15);
	    ch[22] = ch[20]>>1;
	    ch[23] = ch[21]>>1;
	    ch[18] += ch[22];
	    ch[19] += ch[23];
        printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			rb,pil[10],pil[11],rxF[16],rxF[17],ch[20],ch[21]);
	      pil+=12;
	      ch+=24;
	      rxF+=24;
	  // ________________________Second half of RBs____________________	  
	    for (rb=0;rb<11;rb++) {
	      // ------------------------1st pilot---------------------------
	      ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	      ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	      ch[2] = ch[0]>>1;
	      ch[3] = ch[1]>>1;		   
		  ch[-2] += ch[2];
		  ch[-1] += ch[3];
		 
	      printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
		  // ------------------------2nd pilot------------------------
	      ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	      ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	      ch[6] = ch[4]>>1;
	      ch[7] = ch[5]>>1;
	      ch[2] += ch[6];
	      ch[3] += ch[7];
	      printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);  
	       // ------------------------3rd pilot------------------------
	      ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	      ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	      ch[10] = ch[8]>>1;
	      ch[11] = ch[9]>>1;
	      ch[6] += ch[10];
	      ch[7] += ch[11];
	      printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]);
	      // ------------------------4th pilot------------------------
	      ch[12] = (short)(((int)pil[6]*rxF[24] - (int)pil[7]*rxF[25])>>15);
	      ch[13] = (short)(((int)pil[6]*rxF[25] + (int)pil[7]*rxF[24])>>15);
	      ch[14] = ch[12]>>1;
	      ch[15] = ch[13]>>1;
	      ch[10] += ch[14];
	      ch[11] += ch[15];
	      printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[6],pil[7],rxF[24],rxF[25],ch[12],ch[13]);
	       // ------------------------5th pilot------------------------
	      ch[16] = (short)(((int)pil[8]*rxF[32] - (int)pil[9]*rxF[33])>>15);
	      ch[17] = (short)(((int)pil[8]*rxF[33] + (int)pil[9]*rxF[32])>>15);
	      ch[18] = ch[16]>>1;
	      ch[19] = ch[17]>>1;
	      ch[14] += ch[18];
	      ch[15] += ch[19];
	      printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[32],rxF[33],ch[16],ch[17]);
	      // ------------------------6th pilot------------------------
	      ch[20] = (short)(((int)pil[10]*rxF[40] - (int)pil[11]*rxF[41])>>15);
	      ch[21] = (short)(((int)pil[10]*rxF[41] + (int)pil[11]*rxF[40])>>15);
	      ch[22] = ch[20]>>1;
	      ch[23] = ch[21]>>1;
	      ch[18] += ch[22];
	      ch[19] += ch[23];
	      printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[10],pil[11],rxF[40],rxF[41],ch[20],ch[21]);
	      pil+=12;
	      ch+=24;
	      rxF+=48;
	    }	
		 // ------------------------Last PRB ---------------------------
		 // ------------------------1st pilot---------------------------
	      ch[0] = (short)(((int)pil[0]*rxF[0] - (int)pil[1]*rxF[1])>>15);
	      ch[1] = (short)(((int)pil[0]*rxF[1] + (int)pil[1]*rxF[0])>>15);
	      ch[2] = ch[0]>>1;
	      ch[3] = ch[1]>>1;		   
		  ch[-2] += ch[2];
		  ch[-1] += ch[3];		 
	      printf("rb %d: pil0 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[0],pil[1],rxF[0],rxF[1],ch[0],ch[1]);
		  // ------------------------2nd pilot------------------------
	      ch[4] = (short)(((int)pil[2]*rxF[8] - (int)pil[3]*rxF[9])>>15);
	      ch[5] = (short)(((int)pil[2]*rxF[9] + (int)pil[3]*rxF[8])>>15);
	      ch[6] = ch[4]>>1;
	      ch[7] = ch[5]>>1;
	      ch[2] += ch[6];
	      ch[3] += ch[7];
	      printf("rb %d: pil1 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[2],pil[3],rxF[8],rxF[9],ch[4],ch[5]);  
	       // ------------------------3rd pilot------------------------
	      ch[8] = (short)(((int)pil[4]*rxF[16] - (int)pil[5]*rxF[17])>>15);
	      ch[9] = (short)(((int)pil[4]*rxF[17] + (int)pil[5]*rxF[16])>>15);
	      ch[10] = ch[8]>>1;
	      ch[11] = ch[9]>>1;
	      ch[6] += ch[10];
	      ch[7] += ch[11];
	      printf("rb %d: pil2 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[16],rxF[17],ch[8],ch[9]);
	      // ------------------------4th pilot------------------------
	      ch[12] = (short)(((int)pil[6]*rxF[24] - (int)pil[7]*rxF[25])>>15);
	      ch[13] = (short)(((int)pil[6]*rxF[25] + (int)pil[7]*rxF[24])>>15);
	      ch[14] = ch[12]>>1;
	      ch[15] = ch[13]>>1;
	      ch[10] += ch[14];
	      ch[11] += ch[15];
	      printf("rb %d: pil3 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[6],pil[7],rxF[24],rxF[25],ch[12],ch[13]);
	       // ------------------------5th pilot------------------------
	      ch[16] = (short)(((int)pil[8]*rxF[32] - (int)pil[9]*rxF[33])>>15);
	      ch[17] = (short)(((int)pil[8]*rxF[33] + (int)pil[9]*rxF[32])>>15);
	      ch[18] = ch[16]>>1;
	      ch[19] = ch[17]>>1;
	      ch[14] += ch[18];
	      ch[15] += ch[19];
	      printf("rb %d: pil4 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[4],pil[5],rxF[32],rxF[33],ch[16],ch[17]);
	      // ------------------------6th pilot------------------------
	      ch[20] = (short)(((int)pil[10]*rxF[40] - (int)pil[11]*rxF[41])>>15);
	      ch[21] = (short)(((int)pil[10]*rxF[41] + (int)pil[11]*rxF[40])>>15);
	      ch[22]= (ch[20]>>1)*3- ch[18];
		  ch[23]= (ch[21]>>1)*3- ch[19];		      
	      ch[18] += ch[20]>>1;
	      ch[19] += ch[21]>>1;
		    printf("rb %d: pil5 (%d,%d) x (%d,%d) = (%d,%d)\n",
			13+rb,pil[10],pil[11],rxF[40],rxF[41],ch[20],ch[21]);
	    }
	  //------------------------Temporal Interpolation ------------------------------
	  if (l=6) {		  
	   ch = (short *)&dl_ch_estimates[aarx][ch_offset];  	  
	  //      printf("Interpolating %d->0\n",4-phy_vars_ue->lte_frame_parms.Ncp);
	  ch_prev = (short *)&dl_ch_estimates[aarx][2*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)];
	  ch0 = (short *)&dl_ch_estimates[aarx][0*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)];
	  memcpy(ch0,ch_prev,4*phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
		 
	  ch1 = (short *)&dl_ch_estimates[aarx][1*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)];
	  memcpy(ch1,ch_prev,4*phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  
	  // 3/4 ch2 + 1/4 ch6 => ch3
	  multadd_complex_vector_real_scalar(ch_prev,24576,ch_prev+(2*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),1,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  multadd_complex_vector_real_scalar(ch,8192,ch_prev+(2*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),0,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  // 1/2 ch2 + 1/2 ch6 => ch4
	  multadd_complex_vector_real_scalar(ch_prev,16384,ch_prev+(4*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),1,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  multadd_complex_vector_real_scalar(ch,16384,ch_prev+(4*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),0,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  // 1/4 ch2 + 3/4 ch6 => ch5
	  multadd_complex_vector_real_scalar(ch_prev,8192,ch_prev+(6*((phy_vars_ue->lte_frame_parms.ofdm_symbol_size))),1,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  multadd_complex_vector_real_scalar(ch,24576,ch_prev+(6*((phy_vars_ue->lte_frame_parms.ofdm_symbol_size))),0,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
    }
	 if (l=10) { 
	  ch = (short *)&dl_ch_estimates[aarx][ch_offset];    
      ch_prev = (short *)&dl_ch_estimates[aarx][6*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)];
	  // 3/4 ch6 + 1/4 ch10 => ch7
	  multadd_complex_vector_real_scalar(ch_prev,24576,ch_prev+(2*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),1,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  multadd_complex_vector_real_scalar(ch,8192,ch_prev+(2*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),0,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  // 1/2 ch6 + 1/2 ch10 => ch8
	  multadd_complex_vector_real_scalar(ch_prev,16384,ch_prev+(4*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),1,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  multadd_complex_vector_real_scalar(ch,16384,ch_prev+(4*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)),0,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  // 1/4 ch6 + 3/4 ch10 => ch9
	  multadd_complex_vector_real_scalar(ch_prev,8192,ch_prev+(6*((phy_vars_ue->lte_frame_parms.ofdm_symbol_size))),1,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
	  multadd_complex_vector_real_scalar(ch,24576,ch_prev+(6*((phy_vars_ue->lte_frame_parms.ofdm_symbol_size))),0,phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
      // 5/4 ch10 - 1/4 ch6 => ch11	
	  // Ch11
	  ch_prev = (short *)&dl_ch_estimates[aarx][10*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)];
	  ch11 = (short *)&dl_ch_estimates[aarx][11*(phy_vars_ue->lte_frame_parms.ofdm_symbol_size)];
	  memcpy(ch11,ch_prev,4*phy_vars_ue->lte_frame_parms.ofdm_symbol_size);
  }
	}
	else if (phy_vars_ue->lte_frame_parms.N_RB_DL==15) {
	// Interpolation  and extrapolation;	  

      } 
      else {
	msg("channel estimation not implemented for phy_vars_ue->lte_frame_parms.N_RB_DL = %d\n",phy_vars_ue->lte_frame_parms.N_RB_DL);
      }
  } 
    // do ifft of channel estimate
      for (aa=0;aa<phy_vars_ue->lte_frame_parms.nb_antennas_rx*phy_vars_ue->lte_frame_parms.nb_antennas_tx;aa++) {
	if (phy_vars_ue->lte_ue_common_vars.dl_ch_estimates[eNB_offset][aa])
	  fft((short*) &phy_vars_ue->lte_ue_common_vars.dl_ch_estimates[eNB_offset][aa][LTE_CE_OFFSET],
	      (short*) phy_vars_ue->lte_ue_common_vars.dl_ch_estimates_time[eNB_offset][aa],
	      phy_vars_ue->lte_frame_parms.twiddle_ifft,
	      phy_vars_ue->lte_frame_parms.rev,
	      phy_vars_ue->lte_frame_parms.log2_symbol_size,
	      phy_vars_ue->lte_frame_parms.log2_symbol_size/2,
	      0);
      }
  
      return(0); 
    }

