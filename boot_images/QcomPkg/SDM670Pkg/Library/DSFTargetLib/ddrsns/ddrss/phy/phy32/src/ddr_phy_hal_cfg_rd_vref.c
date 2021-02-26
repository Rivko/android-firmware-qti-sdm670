//===========================================================================
//  Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
//  QUALCOMM Proprietary and Confidential. 
//===========================================================================
////////////////////////////////////////////////////////////////////////////////////////////////

#include "ddr_phy.h"
#include "ddrss.h"
void DDR_PHY_hal_cfg_local_vref( uint32 _inst_, uint16 cs, uint32 slice, uint32 vref_val )
{
    switch (cs) {
        case 0 : switch (slice) {
                    case  0: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE0_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  1: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE1_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  2: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE2_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  3: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE3_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  4: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE4_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  5: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE5_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  6: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE6_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  7: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE7_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  8: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE8_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case  9: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE9_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R0, vref_val); break;
                    case 10: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE10_DQS_LOCAL_VREF_CFG, VREF_TRIM_R0, vref_val); break;
                }//slice
                break;// rank0
        case 1 : switch (slice) {
                    case  0: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE0_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  1: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE1_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  2: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE2_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  3: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE3_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  4: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE4_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  5: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE5_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  6: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE6_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  7: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE7_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  8: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE8_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case  9: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE9_DQ_LOCAL_VREF_CFG,   VREF_TRIM_R1, vref_val); break;
                    case 10: HWIO_OUTXF (_inst_ , DDR_PHY_DDRPHY_CMSLICE10_DQS_LOCAL_VREF_CFG, VREF_TRIM_R1, vref_val); break;
                }//slice
                break;// rank1
    }//cs
}

void DDR_PHY_hal_cfg_global_vref_hprx( uint32 _inst_, uint32 vref_val)
{
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, GLOBAL_HP_VREF_TRIM      , vref_val);
}

uint8 DDR_PHY_hal_get_global_vref_hprx( uint32 _inst_)
{
   return( HWIO_INXF (_inst_, DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, GLOBAL_HP_VREF_TRIM ));
}

const unsigned int DDRPHY_GLOBALVREFEXT_PRFS_CFG_ADDR[] = 
{
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_1_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_2_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_3_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_4_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_5_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_6_CFG),
	HWIO_ADDRX(0, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_7_CFG),
};

const unsigned int GLOBAL_xP_VREF_TRIM_MASK[] = 
{
	HWIO_FMSK(DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG, GLOBAL_MP_VREF_TRIM),
	HWIO_FMSK(DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG, GLOBAL_HP_VREF_TRIM)
};

const unsigned int GLOBAL_xP_VREF_TRIM_MASK_SHIFT[] = 
{
	HWIO_SHFT(DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG, GLOBAL_MP_VREF_TRIM),
	HWIO_SHFT(DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG, GLOBAL_HP_VREF_TRIM)
};


int DDR_PHY_hal_get_rd_vref_PRFS( const uint32 _inst_, const uint32 prfs_x, const uint8 mp_hp)
{
	if ( (prfs_x >= sizeof(DDRPHY_GLOBALVREFEXT_PRFS_CFG_ADDR)/sizeof(DDRPHY_GLOBALVREFEXT_PRFS_CFG_ADDR[0]))
		|| (mp_hp >= sizeof(GLOBAL_xP_VREF_TRIM_MASK)/sizeof(GLOBAL_xP_VREF_TRIM_MASK[0])) )
		{
			return -1;
		}
	return (in_dword_masked( _inst_ + DDRPHY_GLOBALVREFEXT_PRFS_CFG_ADDR[prfs_x], GLOBAL_xP_VREF_TRIM_MASK[mp_hp]) >> GLOBAL_xP_VREF_TRIM_MASK_SHIFT[mp_hp]);
}

int  DDR_PHY_hal_cfg_global_vref_hprx_PRFS( uint32 _inst_,uint32 prfs_x, uint8 vref_val)
{
	switch (prfs_x){
	    case  7 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_7_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
				  break;  
	    case  6 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_6_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
		          break; 
	    case  5 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_5_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
		          break; 
	    case  4 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_4_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  3 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_3_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  2 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_2_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  1 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_1_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  0 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG, GLOBAL_HP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    default: return (-1);
	}
}

int  DDR_PHY_hal_cfg_global_vref_mprx_PRFS( uint32 _inst_,uint32 prfs_x, uint8 vref_val)
{
	switch (prfs_x){
	    case  7 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_7_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
				  break;  
	    case  6 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_6_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
		          break; 
	    case  5 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_5_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
		          break; 
	    case  4 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_4_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  3 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_3_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  2 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_2_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  1 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_1_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    case  0 : HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_GLOBALVREFEXT_PRFS_0_CFG, GLOBAL_MP_VREF_TRIM, vref_val);
		          return (1);
				  break; 
	    default: return (-1);
	}
}




//void DDR_PHY_hal_cfg_global_vref_hprx_entry( uint32 _inst_)
//{
//    uint32 temp;
//    
//    // Set decouple bit (RMW).This sets the IOqualifier to MPVREF. This is required durining training. bit[0] = 0 => MPRX
//    temp = HWIO_INX (_inst_, DDR_PHY_DDRPHY_PAD_DQS_1_CFG);
//    temp = temp | 2;//set bit 2 to ONE
//    HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_PAD_DQS_1_CFG, temp);
//}
//void DDR_PHY_hal_cfg_global_vref_hprx_exit ( uint32 _inst_)
//{
//        uint32 temp;
//
//    // Restore decouple bit.
//    temp = HWIO_INX (_inst_, DDR_PHY_DDRPHY_PAD_DQS_1_CFG);
//    temp = temp & 0xFFFFFFFD;//set bit 2 to ZERO
//    HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_PAD_DQS_1_CFG, temp);
//}


void DDR_PHY_hal_cfg_global_vref_mprx( uint32 _inst_, uint32 vref_val)
{
    //both CSRs set to same value because IO is too slow for rank switching
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, GLOBAL_MP_VREF_TRIM,vref_val);
}

uint8 DDR_PHY_hal_get_global_vref_mprx( uint32 _inst_)
{
   return( HWIO_INXF (_inst_, DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, GLOBAL_MP_VREF_TRIM ));
}



int DDR_PHY_hal_dqsdcc_read (uint32 _inst_, uint8 rank, int dcc_adj)
{
    int dcc_edge = 0;
    int cdc_edge = 0;

    //dcc_adj= even_anchor-odd_anchor;
    dcc_edge=0;
    cdc_edge=1;// use the ODD histogram for perbit training
    if (dcc_adj< 0)
    {         
        dcc_adj= -dcc_adj;
        dcc_edge=1;
        cdc_edge=0;// use the EVEN histogram for perbit training
    }
    if (9 < dcc_adj)
    {
        dcc_adj  = 9;
        cdc_edge = 2;  // use the BOTH histogram for perbit training
    }
    
    
    if(rank==0)    
    {
        HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG  ,DCC_CTL  ,dcc_adj<<(dcc_edge * 4));
    }
    else
    {
        HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_1_CFG,DCC_CTL_R1,dcc_adj<<(dcc_edge * 4));
    }
    
    return cdc_edge;
}      
 