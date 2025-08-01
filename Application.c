/*
 * Application.c
 *
 *  Created on: 07-Feb-2017
 *      Author: Gajendra
 */

#define applicationDef

#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_icu.h"
#include "r_cg_port.h"
#include "r_cg_cmt.h"
#include "r_cg_rtc.h"
#include "r_cg_sci.h"
#include "r_cg_rspi.h"
#include "r_cg_userdefine.h"
#include "r_cg_iwdt.h"

#include "string.h"
#include "stdio.h"

#include "appDefines.h"
#include "Application.h"
#include "flash_app.h"
#include "PCE_EXTFLASH.h"
#include "PCE_KEYPAD.h"
#include "PCE_LCD.H"
#include "PCE_PRINTER.h"
#include "PCE_UTFPRINTER.h"
#include "PCE_RTC.H"
#include "PCE_SERIAL.h"
#include "PCE_UTILS.h"
#include "PCE_MODEM.h"
#include "appTypeDefs.h"
#include "Union.h"
#include "MemoryFunctions.h"
#include "Settings.h"
#include "Reports.h"
#include "usb_hmsc_apl.h"

#include "../hcc/src/psp/board/init/target.h"
#include "../hcc/src/psp/board/demo/demo.h"
#include "../hcc/src/psp/target/usb-otg/psp_usb_otg.h"

//-------------------------------------------------------------------------
//Unions
//-------------------------------------------------------------------------
union unionProductSummaryDetails product_summary = {0};
union unionMachineDetails mc={0};
union ers ers = {0};
union unionRecordDetails rec = {0};
union unionTempRecordDetails temprec = {0};
union uniondaily daily = {0};
union unionMonthly monthly = {0};
union unionYearly yearly = {0};
union unionSlab slab = {0};
union unionUDFNo udfno = {0};
union unionHotKey hot_key = {0};
union unionConfig config = {0};
union unionSGSTSummaryDetails sgst_summary = {0};
union unionCGSTSummaryDetails cgst_summary = {0};
union unionIGSTSummaryDetails igst_summary = {0};
union unionUDFShareNo udfshno = {0};
union unionBarcodeDetails barcode_details[MAX_PRODUCTS + 1] = {0};
union unionUDFProdSale udf_prd_cnt = {0};
union unionGSMGPRS gsm_gprs = {0};
union unionProductNameArr productNameArr[MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH+2] = {0};
union unionProductStockDetails product_stock = {0};
union uniondaily rpt_daily = {0};
union unionMonthly rpt_monthly = {0};
union unionYearly rpt_yearly = {0};
union unionWSSett ws_sett = {0};
union unionCustomerDetails cust_details = {0};
union unionMCPage mc_pos = {0};
union unionReportDetails rpt_details = {0};
union unionBWR bwr = {0};
union unionIWR iwr = {0};
union unionUDFRec udfrec = {0};
union  unionMobileDetails mob_details = {0};

union unionTempProductSummaryDetails temp_product_summary[MAX_PRODUCTS + 5] = {0};
union unionTempProductStockDetails temp_product_stock[MAX_PRODUCTS + 5] = {0};
union unionTaxSummaryDetails SGST_tax_summary[TEMP_TAX_SUMM_ARRAY + 5] = {0};
union unionTaxSummaryDetails CGST_tax_summary[TEMP_TAX_SUMM_ARRAY + 5] = {0};
union unionTaxSummaryDetails IGST_tax_summary[TEMP_TAX_SUMM_ARRAY + 5] = {0};
union unionSaveRecordDetails save_rec[TEMP_RECS_DISC_ARRAY] = {0};
union unionProdArray prd_array[TEMP_REC_ARRAY_SIZE] = {0};

union unionRecStockArray rec_stock[TEMP_REC_ARRAY_SIZE] = {0};

union unionTableMap tablemap = {0};  //vikas
union unionTableArray tablearray[TABLE_ARRAY_SIZE] = {0}; //vikas

union unionSlabNameDetails slab_name = {0};

union unionTWR twr = {0};
union unionTWR_ptr twr_ptr = {0};


union unionReportDetailscount rpt_details_count = {0};
//-------------------------------------------------------------------------
//Extern Variables
//-------------------------------------------------------------------------
extern union unionTime tim;
extern volatile unsigned char double_width, double_height;
extern volatile unsigned char get_dig_dot_prn_key, get_dig_dot_esc_key,recordedDotPosition, get_dig_esc_flag, get_dig_dot_shift_key,allow_get_dig_dot_shift_key, allow_rate_sel;
extern volatile unsigned char blink_flag, doubleKeyPressCheck,doubleKeyPressCheckWait, shiftFlag;
extern volatile unsigned long long value_hex_asc_64U;
extern unsigned char dd2[2], mm2[2], yy2[2], hr2[2], mn2[2], sc2[2];
extern unsigned char RXD_CHAR;
extern unsigned char StatusLedColor;
extern unsigned char StatusLedFlash;
extern volatile unsigned char hotKeyConfigureFlag, hotKey, hotKeyShift;
extern char modem_connectivity;
extern volatile unsigned long printerIdle;
extern volatile unsigned char move_frwd_flag;
//extern volatile char modem_rx_buff[512];
//-------------------------------------------------------------------------
//Global Variables
//-------------------------------------------------------------------------
UINT16  udf_no = 0, slab_no = 1;
volatile UCHAR udf_char = 0;
UCHAR print_mode = 0;
UINT16 countSI = 0, operatorCode = 0, operatorPos = 0;
volatile unsigned short int product_code = 0;//AKSHAY UINT16 product_code = 0;
//UINT32 final_bill_amount = 0, startRec = 0, endRec = 0;
UINT64 final_bill_amount = 0, startRec = 0, endRec = 0;		//Mrugnil 30/01/24
UINT64 localAmount = 0;
UINT32 prod_discount = 0, prod_vat = 0, qty = 0;
UCHAR unit_type = 0, unit_fract = 0;
UINT64 pre_roundoff = 0, post_roundoff = 0;
UINT64 recevied_cash = 0, credit_amount = 0, returned_amount = 0, pretax_discount_rupee = 0;
UINT32 selectedQty = 0;
UINT64 selectedAmount = 0;
UINT64 last_bill_amount = 0;
UINT32 temp_start = 0, temp_end = 0, last_rec_start = 0, last_rec_end = 0;
UINT32 selectedProd = 0, startProd = 0, lastRecReceiptNo = 0;
UINT32 returnBillReceiptNo = 0;
UCHAR tempArr[6] = {0}, paymentType = 0;
volatile unsigned char asc[200], membuf[BUF_LEN]={0}, temp_buff[MAR_MAX_CHARS] = {0};
volatile unsigned char lowBatteryError = 0, criticalLowError = 0;
volatile UINT16 sgstArray[TEMP_REC_ARRAY_SIZE], cgstArray[TEMP_REC_ARRAY_SIZE];
volatile UINT16 sgstArrayPtr = 0, cgstArrayPtr = 0;
volatile unsigned returnPageFlag = 0, returnDay = 0;
unsigned char g_customerName[CUSTOMER_NAME_LEN], g_GST_IN[GSTIN_NO_LEN];
unsigned int gprsFlag_ptr = 0;
UINT16 g_prodPosArrayPtr = 0, g_prodArrayPtr = 0;
UINT16 g_ID_buf[MAX_PRODUCTS + 1] = {0};
unsigned char g_8K_membuf[8200];
UINT16  g_returnCount = 0, g_actualCount = 0;
volatile UCHAR flagIGST = 0;
volatile unsigned char mobileNumber[MOBILE_NUMBER_LEN + 1] = {0};
UCHAR edit_kot_qty_flag = 0, edit_kot_record_start = 0, edit_kot_record_end = 0;//vikas
UINT16 edit_kot_qty_selected = 0; //vikas

//const unsigned char version[] = {"BBP3T GPRS-N v1.21.8"};
const unsigned char version[] = 	{" BBPX 3T SUPER STAR "};
const unsigned char version1[] = 	{" NORMAL OC V1.01.33 "};		//Mrugnil 20/06/25
const unsigned char version_date[] = {__DATE__};
const unsigned char version_time[] = {__TIME__};

//volatile unsigned int recArrayPtr = 0;
volatile unsigned long recArrayPtr = 0;		//Mrugnil 30/01/24
volatile unsigned char recArray[TEMP_REC_ARRAY_SIZE][RECDAT_LEN];
volatile unsigned char returnBill = 0;
volatile unsigned char recStockPtr = 0;

unsigned int BATT_VOLTAGE = 0;
unsigned int BATT_PERCENTAGE = 0;
unsigned int BATT_CURRENT = 0;
unsigned int USB_VOLTAGE = 0;

volatile unsigned char loginProcess = 0;

unsigned char BatteryFull = 0;
volatile unsigned char loginFlag = 0, managerLogin = 0, calculatorMode = 0, searchProductSingle = 0;

short int product_bacode_checksum = 0;//nilesh
char product_bacode[30] = {0};//nilesh
char barcode_entry_flag = 0;//nilesh
char barcode_arr[30] = {0};//nilesh
int barcode_count = 0;		//nilesh
extern int barcode_count;//nilesh
extern unsigned char logo_print;		//Mrugnil 12/01/24

volatile unsigned char applicationInitCheck = 0, callReceived = 0;
volatile UINT64 voidTotalAmount = 0;
volatile UINT32 voidRecs = 0;

extern unsigned char SecID_Buffer[48];
volatile unsigned char temp_dd = 0, temp_mm = 0, temp_yy = 0;
static unsigned char PrevBattLevel = 0xFF;
//-------------------------------------------------------------------------
extern volatile unsigned long time_out,time_out1, batteryTimeOut,keyPressTimeOut, tout2_disp_network, tout2_simCheck;;
extern volatile unsigned char rec_flag,weighingScaleReceiveFlag,weighingStartFlag;
extern volatile unsigned long Vikas_Timer; //vikas

extern unsigned char	simSTAT;

volatile UCHAR EXT_PRINTER_FLAG = 0;
volatile unsigned char MODEM_BUSY = 0;
volatile unsigned char MODEM_ONCE = 0;
volatile unsigned char	signal_strength_index = 0, ErrorCsq;


extern volatile unsigned char modem_rx_done;
extern volatile char modem_rx_buff[2048];
extern volatile unsigned long modemTimeOut;
extern volatile unsigned long modemPushTimeOut;

extern volatile unsigned char insideQuantityFunction;		//Mrugnil 31/01/24

unsigned char deletedProduct = 0;		//Mrugnil 27/06/24

//-------------------------------------------------------------------------
extern void PowerON_Reset_PC(void);
int main (void)
{
	unsigned char key = 0, loop = 0;
	unsigned int i = 0;
	UINT32 recs = 0;

	UserInit();

	check3INCHSecurityCode();
	FlashReadDevID();

	senseBatteryVoltage();

	ReadG1CBatt();
	if(criticalLowError)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(3,1);
		lcdprint("CRITICAL BATTERY LOW");
		gotoxy(5,1);
		lcdprint("   PLEASE SHUTDOWN  ");
		beep(LONGBEEP);
		Delay_ms(2000);
		while(criticalLowError)
		{
			keyPressCheck();
		}
		clrscr();
	}

	if(lowBatteryError)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(3,1);
		lcdprint("    LOW BATTERY!    ");
		gotoxy(5,1);
		lcdprint("   CONNECT CHARGER  ");
	}

	for(i = 0; i <= MAX_PRODUCTS; i++)
	{
		memset(temp_product_summary[i].arr, 0, PRODUCT_SUMMARY_LEN);
	}

	for(i = 0; i <= MAX_PRODUCTS; i++)
	{
		memset(temp_product_stock[i].arr, 0, PRODUCT_STOCK_UNION_LEN);
	}

	for(i = 0; i < TEMP_TAX_SUMM_ARRAY; i++)
	{
		memset(SGST_tax_summary[i].arr, 0, TAX_SUMMARY_LEN);
		memset(CGST_tax_summary[i].arr, 0, TAX_SUMMARY_LEN);
		memset(IGST_tax_summary[i].arr, 0, TAX_SUMMARY_LEN);
	}

	for(i = 0; i < TEMP_REC_ARRAY_SIZE; i++)
	{
		memset(rec_stock[i].arr, 0, REC_STOCK_LEN);
	}

	get_mc_position();
	restore_mc();
	beep(LONGBEEP);

	checkFailedSwitchOff();

	mc.mc.checkIntervalFail = 0;
	mc.mc.checkOffByte = 0;
	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;
	mc.mc.checkStartRecAddress = recs;
	store_mc();

	get_mc_position();
	get_mc();

	get_twr_ptr();

	if((mc.mc.lcd_contrast > 90) || (mc.mc.lcd_contrast < 80))
	{
		mc.mc.lcd_contrast = 86;
		store_mc();
	}

	Delay_ms(200);		//Mrugnil 06/04/24
	set_lcd_contrast(mc.mc.lcd_contrast);
	Delay_ms(300);		//Mrugnil 05/04/24

//	if(mc.mc.weighing_scale)
//	{
		checkBaudRate();
//	}

	blon();
	clrscr();
	double_height = 1;
	double_width = 1;
	gotoxy(3,1);
	lcdprint("  BALAJI  ");

	double_height = 1;
	double_width = 0;
	gotoxy(5,1);
	lcdprint("  BILLING  PRINTER  ");

    initModem();

    sendURLToModem();
    modemTimeOut = 0;
    while(modemTimeOut < 1999);
    Delay_ms(50);
    changeAPN();
    modemTimeOut = 0;

	Delay_ms(1000);
	PrevBattLevel = 0xFF;
	ReadG1CBatt();
	get_mass_storage_barcode_usb_keyboard_settings();
	get_barcode_details(0);//nilesh
	get_product_names();//nilesh
	load256ProductSummaryinTempArray(1);
	loadProductsStockTempArray(1);
	loadTaxSummaryinTempArray(0, mc.mc.sgst_summ_count, SGST, mc.mc.currentPG);
	loadTaxSummaryinTempArray(0, mc.mc.cgst_summ_count, CGST, mc.mc.currentPG);
	loadTaxSummaryinTempArray(0, mc.mc.igst_summ_count, IGST, mc.mc.currentPG);
	loadTableMapContents();
	clrscr();

	applicationInitCheck = 1;
	if(mc.mc.gprs_en_dis|mc.mc.gsm_en_dis)
	{
		get_gsm_gprs();
		simSTAT = check_sim_card_exists();
		if(!simSTAT)
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3,1);
			lcdprint("SIM CARD NOT PRESENT");
			beep(LONGBEEP);
			Delay_ms(2000);
		}
		else
		{
			stopPostCMDToModem();
			check_signal_strength();
		}
	}
	else
	{
		modemPower(0);
	}

    while (1U)
    {
    	while(!(key = keyPressCheck()) && (barcode_entry_flag == 0))
		{
			check3INCHSecurityCode();
			homeScreen();
			checkDateChange();
			turnOffPrinter();
			if(!recArrayPtr)
			{
				if(mc.mc.gsm_en_dis == CALL_WISE)
				{
					if(callReceived)
					{
						getCallDetails();
					}
				}
				checkHourandSendData();
				if(mc.mc.gprs_en_dis)
				{
					if(modemPushTimeOut > 14999)
					{
						modemPushTimeOut = 0;
						stopPostCMDToModem();
						pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
						sendAllDataCMDToModem();
					}
				}
			}
			if(criticalLowError)
			{
				clrscr();
				double_height = 1;
				double_width = 0;
				gotoxy(3,1);
				lcdprint("CRITICAL BATTERY LOW");
				gotoxy(5,1);
				lcdprint("   PLEASE SHUTDOWN  ");
				beep(LONGBEEP);
				Delay_ms(2000);
				while(criticalLowError)
				{
					keyPressCheck();
				}
				clrscr();
			}
			chk_comm();

			if(usb_barcode_enable == TRUE)
			{
				product_bacode_checksum = 0;
				if(TRUE == usb_barcode_available)//nilesh
				{
					strcpy(product_bacode,barcode_arr);
					memset(barcode_arr,0,sizeof(barcode_arr));
					barcode_count = 0;
					usb_barcode_available = FALSE;
					for(loop = 0 ; loop < strlen(product_bacode) ; loop++)
						product_bacode_checksum += product_bacode[loop];

					barcode_entry_flag = 1;
					break;
				}
			}
		}
		sel_func(key);
		barcode_entry_flag = 0;//nilesh
		product_bacode_checksum = 0;
		memset(product_bacode, 0, sizeof(product_bacode));
    }
}
//--------------------------------------------------------------------------
void check3INCHSecurityCode(void)
{
	FlashReadSecID();

	if((SecID_Buffer[PRINTER_TYPE] != THREE_INCH))
	{
		if(SecID_Buffer[PRINTER_TYPE] == 0xFF)
			return;
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(2,1);
		lcdprint(" THIS BOARD DOES NOT");
		gotoxy(4,1);
		lcdprint(" SUPPORT 3INCH VERS.");
		beep(ERRRBEEP);
		double_height = 0;
		gotoxy(7,1);
		lcdprint("  PLEASE SWITCH OFF ");

		while(1)
		{
			CheckOnOffKey();
		}
	}
}
//--------------------------------------------------------------------------
void checkBaudRate(void)
{
	clrscr();
	gotoxy(4,1);
	double_height = 1;
	double_width = 0;
	lcdprint(" CHECKING BAUD RATE ");
	switch(mc.mc.baudrate)
	{
		case 1:
					changeBaudRate(BR115200);
					break;
		case 2:
					changeBaudRate(BR19200);
					break;
		case 3:
					changeBaudRate(BR9600);
					break;
		case 4:
					changeBaudRate(BR2400);
					break;
		case 5:
					changeBaudRate(BR4800);
					break;
	}
}
//--------------------------------------------------------------------------
void senseBatteryVoltage(void)
{
	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(3,1);
	lcdprint("   SENSING BATTERY  ");
	double_height = 0;
	double_width = 0;
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT...   ");

	ReadG1CBatt();
	batteryTimeOut = 0;
	while((BATT_VOLTAGE == 0)&&(batteryTimeOut < 10000))
	{
		ReadG1CBatt();
	}
	clrscr();
}
//--------------------------------------------------------------------------
void checkFailedSwitchOff(void)
{
	UINT32 loopCount = 0, recs = 0, count = 1, loop = 0;
	UCHAR entry = 0, addFlag = 0;
	clrscr();

	get_mc();

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	if(recs > 1)
	{
		if(mc.mc.checkOffByte != SWITCH_OFF_BYTE)
		{
			double_height = 1;
			double_width = 0;

			gotoxy(1,1);
			lcdprint(" PROCESSING RECORDS ");
			gotoxy(4,1);
			lcdprint(" RECORD : ");
			get_daily_summary(mc.mc.current_dd);
			get_monthly_summary(mc.mc.current_mm);
			get_yearly_summary(mc.mc.current_yy);
			for(loopCount = mc.mc.checkStartRecAddress; loopCount < recs; loopCount++)
			{
				gotoxy(4,11);
				hex_asc(count, asc, 5);
				rem_zeros(asc, 4);
				disp_char(asc[0]);
				disp_char(asc[1]);
				disp_char(asc[2]);
				disp_char(asc[3]);
				disp_char(asc[4]);
				count++;

				get_rec(loopCount);

				if((rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last == 1))
				{
					entry = 1;
					addFlag = 1;
				}
				else if((rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 'C') && (rec.rec.last == 1))
				{
					entry = 1;
					addFlag = 1;
				}
				else if((rec.rec.bill_stat == 'V') && (rec.rec.calc_stat == 0) && (rec.rec.last == 1))
				{
					entry = 1;
					addFlag = 0;
				}
				else if((rec.rec.bill_stat == 'V') && (rec.rec.calc_stat == 'C') && (rec.rec.last == 1))
				{
					entry = 1;
					addFlag = 0;
				}
				else if((rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 'R') && (rec.rec.last == 1)
						&& (rec.rec.returnedProduct != 1))
				{
					entry = 1;
					addFlag = 0;
				}
				else if((rec.rec.bill_stat == 'V') && (rec.rec.calc_stat == 'R') && (rec.rec.last == 1)
						&& (rec.rec.returnedProduct != 1))
				{
					entry = 1;
					addFlag = 1;
				}

				if(entry)
				{
					switch(addFlag)
					{
						case 0:
								daily.daily.daily_set_var = 'Y';
								daily.daily.dailyBillNo++;
								if(daily.daily.dailyTotAmnt >= rec.rec.round_off_tot_amt)
									daily.daily.dailyTotAmnt -= rec.rec.round_off_tot_amt;
								else
									daily.daily.dailyTotAmnt = 0;
								if(daily.daily.dailyTotDisc >= rec.rec.discount_amt)
									daily.daily.dailyTotDisc -= rec.rec.discount_amt;
								else
									daily.daily.dailyTotDisc = 0;
								if(daily.daily.dailyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
									daily.daily.dailyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
								else
									daily.daily.dailyTotTax = 0;

								monthly.monthly.monthlyBillNo++;
								if(monthly.monthly.monthlyTotAmnt >= rec.rec.round_off_tot_amt)
									monthly.monthly.monthlyTotAmnt -= rec.rec.round_off_tot_amt;
								else
									monthly.monthly.monthlyTotAmnt = 0;
								if(monthly.monthly.monthlyTotDisc >= rec.rec.discount_amt)
									monthly.monthly.monthlyTotDisc -= rec.rec.discount_amt;
								else
									monthly.monthly.monthlyTotDisc = 0;
								if(monthly.monthly.monthlyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
									monthly.monthly.monthlyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
								else
									monthly.monthly.monthlyTotTax = 0;

								yearly.yearly.yearly_set_var = 'Y';
								yearly.yearly.yearlyBillNo++;
								if(yearly.yearly.yearlyTotAmnt >= rec.rec.round_off_tot_amt)
									yearly.yearly.yearlyTotAmnt -= rec.rec.round_off_tot_amt;
								else
									yearly.yearly.yearlyTotAmnt = 0;
								if(yearly.yearly.yearlyTotDisc >= rec.rec.discount_amt)
									yearly.yearly.yearlyTotDisc -= rec.rec.discount_amt;
								else
									yearly.yearly.yearlyTotDisc = 0;
								if(yearly.yearly.yearlyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
									yearly.yearly.yearlyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
								else
									yearly.yearly.yearlyTotTax = 0;
								break;
						case 1:
								daily.daily.daily_set_var = 'Y';
								daily.daily.dailyBillNo++;
								daily.daily.dailyTotAmnt += rec.rec.round_off_tot_amt;
								daily.daily.dailyTotDisc += rec.rec.discount_amt;
								daily.daily.dailyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);

								monthly.monthly.monthly_set_var = 'Y';
								monthly.monthly.monthlyBillNo++;
								monthly.monthly.monthlyTotAmnt += rec.rec.round_off_tot_amt;
								monthly.monthly.monthlyTotDisc += rec.rec.discount_amt;
								monthly.monthly.monthlyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);

								yearly.yearly.yearly_set_var = 'Y';
								yearly.yearly.yearlyBillNo++;
								yearly.yearly.yearlyTotAmnt += rec.rec.round_off_tot_amt;
								yearly.yearly.yearlyTotDisc += rec.rec.discount_amt;
								yearly.yearly.yearlyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
								break;
					}
				}
				entry = 0;
			}
			store_daily_summary(mc.mc.current_dd);
			store_monthly_summary(mc.mc.current_mm);
			store_yearly_summary(mc.mc.current_yy);

			load256ProductSummaryinTempArray(1);
			loadProductsStockTempArray(1);
			loadTaxSummaryinTempArray(0, mc.mc.sgst_summ_count, SGST, mc.mc.currentPG);
			loadTaxSummaryinTempArray(0, mc.mc.cgst_summ_count, CGST, mc.mc.currentPG);
			loadTaxSummaryinTempArray(0, mc.mc.igst_summ_count, IGST, mc.mc.currentPG);

			double_height = 1;
			double_width = 0;

			gotoxy(1,1);
			lcdprint(" PROCESSING RECORDS ");
			gotoxy(4,1);
			lcdprint(" RECORD : ");
			for(loopCount = mc.mc.checkStartRecAddress; loopCount < recs; loopCount++)
			{
				gotoxy(4,11);
				hex_asc(count, asc, 5);
				rem_zeros(asc, 4);
				disp_char(asc[0]);
				disp_char(asc[1]);
				disp_char(asc[2]);
				disp_char(asc[3]);
				disp_char(asc[4]);
				count++;

				get_rec(loopCount);

				if((rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last != 1))
				{
					entry = 1;
					addFlag = 1;
				}
				else if((rec.rec.bill_stat == 'V') && (rec.rec.calc_stat == 0) && (rec.rec.last != 1))
				{
					entry = 1;
					addFlag = 0;
				}
				else if((rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 'R') && (rec.rec.last != 1)
						&& (rec.rec.returnedProduct != 1))
				{
					entry = 1;
					addFlag = 0;
				}
				else if((rec.rec.bill_stat == 'V') && (rec.rec.calc_stat == 'R') && (rec.rec.last != 1)
						&& (rec.rec.returnedProduct != 1))
				{
					entry = 1;
					addFlag = 1;
				}

				if(entry)
				{
					switch(addFlag)
					{
						case 0:
								if(mc.mc.stock_manage_en_dis)
								{
									if(temp_product_stock[rec.rec.product_code].temp_product_stock.sign == 0)
									{
										temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock = ((rec.rec.quantity/10) + temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock);
										temp_product_stock[rec.rec.product_code].temp_product_stock.sign = 0;
									}
									else
									{
										if(temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock >= (rec.rec.quantity/10))
										{
											temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock -= (rec.rec.quantity/10);
											temp_product_stock[rec.rec.product_code].temp_product_stock.sign = 1;
										}
										else
										{
											temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock = ((rec.rec.quantity/10) - temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock);
											temp_product_stock[rec.rec.product_code].temp_product_stock.sign = 0;
										}
									}
								}

								if(temp_product_summary[rec.rec.product_code].temp_product_summary.total_amount > rec.rec.final_prod_amount)
									temp_product_summary[rec.rec.product_code].temp_product_summary.total_amount -= rec.rec.final_prod_amount;
								else
									temp_product_summary[rec.rec.product_code].temp_product_summary.total_amount = 0;
								if(temp_product_summary[rec.rec.product_code].temp_product_summary.total_qty > rec.rec.quantity)
									temp_product_summary[rec.rec.product_code].temp_product_summary.total_qty -= rec.rec.quantity;
								else
									temp_product_summary[rec.rec.product_code].temp_product_summary.total_qty = 0;
								temp_product_summary[rec.rec.product_code].temp_product_summary.stockBalance = temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock;
								temp_product_summary[rec.rec.product_code].temp_product_summary.stock_sign = temp_product_stock[rec.rec.product_code].temp_product_stock.sign;
								FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) (rec.rec.product_code) * PRODUCT_LEN)), membuf, PRODUCT_LEN);
								temp_product_summary[rec.rec.product_code].temp_product_summary.prodCode = asc_hex(membuf, PRODUCT_SER_NO_LEN);

								if(rec.rec.igstFlag == BILL_IGST)
								{
									for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.igst_summ_count); loop++)
									{
										if(rec.rec.prd_cgst == IGST_tax_summary[loop].tax_summary.rate)
										{
											if(IGST_tax_summary[loop].tax_summary.total_amount > rec.rec.cgst_amt)
												IGST_tax_summary[loop].tax_summary.total_amount -= rec.rec.cgst_amt;
											else
												IGST_tax_summary[loop].tax_summary.total_amount = 0;
										}
									}
								}
								else
								{
									for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.sgst_summ_count); loop++)
									{
										if(rec.rec.prd_sgst == SGST_tax_summary[loop].tax_summary.rate)
										{
											if(SGST_tax_summary[loop].tax_summary.total_amount > rec.rec.sgst_amt)
												SGST_tax_summary[loop].tax_summary.total_amount -= rec.rec.sgst_amt;
											else
												SGST_tax_summary[loop].tax_summary.total_amount = 0;
										}
									}
									for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.cgst_summ_count); loop++)
									{
										if(rec.rec.prd_cgst == CGST_tax_summary[loop].tax_summary.rate)
										{
											if(CGST_tax_summary[loop].tax_summary.total_amount > rec.rec.cgst_amt)
												CGST_tax_summary[loop].tax_summary.total_amount -= rec.rec.cgst_amt;
											else
												CGST_tax_summary[loop].tax_summary.total_amount = 0;
										}
									}
								}
								break;
						case 1:
								if(mc.mc.stock_manage_en_dis)
								{
									if(temp_product_stock[rec.rec.product_code].temp_product_stock.sign == 1)
									{
										temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock = ((rec.rec.quantity/10) + temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock);
										temp_product_stock[rec.rec.product_code].temp_product_stock.sign = 1;
									}
									else
									{
										if(temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock >= (rec.rec.quantity/10))
										{
											temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock -= (rec.rec.quantity/10);
											temp_product_stock[rec.rec.product_code].temp_product_stock.sign = 0;
										}
										else
										{
											temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock = ((rec.rec.quantity/10) - temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock);
											temp_product_stock[rec.rec.product_code].temp_product_stock.sign = 1;
										}
									}
								}

								temp_product_summary[rec.rec.product_code].temp_product_summary.total_amount += rec.rec.final_prod_amount;
								temp_product_summary[rec.rec.product_code].temp_product_summary.total_qty += rec.rec.quantity;
								temp_product_summary[rec.rec.product_code].temp_product_summary.stockBalance = temp_product_stock[rec.rec.product_code].temp_product_stock.totalStock;
								temp_product_summary[rec.rec.product_code].temp_product_summary.stock_sign = temp_product_stock[rec.rec.product_code].temp_product_stock.sign;
								FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) (rec.rec.product_code) * PRODUCT_LEN)), membuf, PRODUCT_LEN);
								temp_product_summary[rec.rec.product_code].temp_product_summary.prodCode = asc_hex(membuf, PRODUCT_SER_NO_LEN);

								if(rec.rec.igstFlag == BILL_IGST)
								{
									for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.igst_summ_count); loop++)
									{
										if(rec.rec.prd_cgst == IGST_tax_summary[loop].tax_summary.rate)
											IGST_tax_summary[loop].tax_summary.total_amount += rec.rec.cgst_amt;
									}
								}
								else
								{
									for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.sgst_summ_count); loop++)
									{
										if(rec.rec.prd_sgst == SGST_tax_summary[loop].tax_summary.rate)
											SGST_tax_summary[loop].tax_summary.total_amount += rec.rec.sgst_amt;
									}
									for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.cgst_summ_count); loop++)
									{
										if(rec.rec.prd_cgst == CGST_tax_summary[loop].tax_summary.rate)
											CGST_tax_summary[loop].tax_summary.total_amount += rec.rec.cgst_amt;
									}
								}
								break;
					}
				}
				entry = 0;
			}
			updateFlashContents();
		}
	}

	get_daily_summary(mc.mc.current_dd);
	get_monthly_summary(mc.mc.current_mm);
	get_yearly_summary(mc.mc.current_yy);
	clrscr();
}
//--------------------------------------------------------------------------
void UserInit(void)
{

	R_CMT1_Start();
	R_CMT3_Start();
	R_RTC_Start();
	R_RSPI0_Start();

    Delay_ms(100);
    display_init();
//    Delay_ms(100);
    Delay_ms(250);		//Mrugnil 05/04/24

	R_SCI9_Start();
	R_SCI9_Serial_Receive(&modem_rx_buff, 1);

	R_SCI2_Start();
	R_SCI2_Serial_Receive(&sci2_rx_char, 1);

	R_SCI5_Start();
	R_SCI5_Serial_Receive(&RXD_CHAR, 1);

	printer_init();
	usb_pins_init();
	psp_usb_otg_set_id(PSP_USB_OTG_ID_HOST);
	initialize_usb();

	switchByteAddessing(ADDR_4BYTE);
//	USB_HMSC_Start();	//Yogesh190517
//	Delay_ms(200);
	clrscr();
}
//-------------------------------------------------------------------------
void PowerOffSystem(void)
{

	if(mc.mc.gprs_en_dis == 1)
	{
		stopPostCMDToModem();
		savePacketToModemUnPostDataBulk(1);
		uploadFilesToServer();
		getUploadedFileDetails();
	}

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(4,1);
	lcdprint("  SHUTTING DOWN...  ");
	double_height = 0;
	gotoxy(6, 1);
	lcdprint("  PLEASE WAIT ....");
	Delay_ms(2000);
	if(applicationInitCheck)
	{
		get_mc();
		mc.mc.checkOffByte = SWITCH_OFF_BYTE;
		if(mc.mc.currentPG)
			mc.mc.checkStartRecAddress = mc.mc.recs;
		else
			mc.mc.checkStartRecAddress = mc.mc.rec1;
		store_daily_summary(mc.mc.current_dd);
		store_monthly_summary(mc.mc.current_mm);
		store_yearly_summary(mc.mc.current_yy);
		store_mc();
		updateFlashContents();
	}
	prn_pow(PRINTER_OFF);
//	USB_HMSC_Stop();	//Yogesh190517
	R_SCI5_Stop();
	R_SCI9_Stop();
	store_backup();

//	if(USB_VOLTAGE > 3)
	if(BATT_CURRENT > 0)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(4,1);
		lcdprint("    CHARGING ...    ");
		Delay_ms(2000);
		display_off();

		while(BatteryFull == 0)
		{
			R_IWDT_Restart();
			ReadG1CBatt();
			StatusLed();
			if(PWR_SW_MON == 0)
			{
				Delay_ms(10);
				if(PWR_SW_MON == 0)
				{
					PowerON_Reset_PC();
				}
			}
//			if(USB_VOLTAGE < 3)
			if(!BATT_CURRENT)
				break;
		}

		if(BatteryFull == 1)
		{
			display_init();
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(4,1);
			lcdprint("  BATTERY CHARGED!  ");
			beep(LONGBEEP);
			Delay_ms(2000);
			beep(LONGBEEP);
			Delay_ms(2000);
			beep(LONGBEEP);	
			Delay_ms(2000);
		}
	}
	
	if(PWR_SW_MON == 0)
	{
//		Delay_ms(10);
		Delay_ms(100);
		if(PWR_SW_MON == 0)
		{
			Delay_ms(100);
			PowerON_Reset_PC();
		}
	}

	display_off();
	PWR_SW_ONOFF_CTRL = 0;
	while(1);
}
//-------------------------------------------------------------------------
void updateFlashContents(void)
{
	UINT32 SUMMARY_START_ADD = 0;
	UINT16 loopProdSumm = 0, loop = 0, loopCount = 0, i = 0;

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	SUMMARY_START_ADD = 0;
	for(loop = 0, loopCount = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.sgst_summ_count); loop++)
	{
		for(i = 0; i < TAX_SUMMARY_LEN; i++)
			g_8K_membuf[loopCount++] = SGST_tax_summary[loop].arr[i];
	}

	if(mc.mc.currentPG)
		FlashModifyData((((unsigned long)(SGST_SUMMARY_PG * PGBYTS)) +
				((unsigned long)(SUMMARY_START_ADD * TAX_SUMMARY_LEN))), g_8K_membuf, loopCount);
	else
		FlashModifyData((((unsigned long)(SGST_SUMMARY1_PG * PGBYTS)) +
				((unsigned long)(SUMMARY_START_ADD * TAX_SUMMARY_LEN))), g_8K_membuf, loopCount);

	SUMMARY_START_ADD = 0;
    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	for(loop = 0, loopCount = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.cgst_summ_count); loop++)
	{
		for(i = 0; i < TAX_SUMMARY_LEN; i++)
			g_8K_membuf[loopCount++] = CGST_tax_summary[loop].arr[i];
	}

	if(mc.mc.currentPG)
		FlashModifyData((((unsigned long)(CGST_SUMMARY_PG * PGBYTS)) +
				((unsigned long)(SUMMARY_START_ADD * TAX_SUMMARY_LEN))), g_8K_membuf, loopCount);
	else
		FlashModifyData((((unsigned long)(CGST_SUMMARY1_PG * PGBYTS)) +
				((unsigned long)(SUMMARY_START_ADD * TAX_SUMMARY_LEN))), g_8K_membuf, loopCount);

	SUMMARY_START_ADD = 0;
    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	for(loop = 0, loopCount = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.igst_summ_count); loop++)
	{
		for(i = 0; i < TAX_SUMMARY_LEN; i++)
			g_8K_membuf[loopCount++] = IGST_tax_summary[loop].arr[i];
	}

	if(mc.mc.currentPG)
		FlashModifyData((((unsigned long)(IGST_SUMMARY_PG * PGBYTS)) +
				((unsigned long)(SUMMARY_START_ADD * TAX_SUMMARY_LEN))), g_8K_membuf, loopCount);
	else
		FlashModifyData((((unsigned long)(IGST_SUMMARY1_PG * PGBYTS)) +
				((unsigned long)(SUMMARY_START_ADD * TAX_SUMMARY_LEN))), g_8K_membuf, loopCount);

	if(mc.mc.stock_manage_en_dis)
	{
	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
		SUMMARY_START_ADD = 1;
		for(loopProdSumm = 0; loopProdSumm < 3; loopProdSumm++)
		{
			for(loop = SUMMARY_START_ADD, loopCount = 0; (loop < (SUMMARY_START_ADD + TEMP_PRODUCT_STOCK_ARRAY)) && (loop <= mc.mc.prod_totalcount); loop++)
			{
				for(i = 0; i < PRODUCT_STOCK_UNION_LEN; i++)
					g_8K_membuf[loopCount++] = temp_product_stock[loop].arr[i];
			}

			FlashModifyData((((unsigned long)(PRODUCT_STOCK_PG * PGBYTS)) +
						((unsigned long)(SUMMARY_START_ADD * PRODUCT_STOCK_UNION_LEN))), g_8K_membuf, loopCount);

		    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

			SUMMARY_START_ADD += TEMP_PRODUCT_STOCK_ARRAY;

			if(SUMMARY_START_ADD >= mc.mc.prod_totalcount)
				break;
		}
	}

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	SUMMARY_START_ADD = 1;
	for(loopProdSumm = 0; loopProdSumm < 8; loopProdSumm++)
	{
		for(loop = SUMMARY_START_ADD, loopCount = 0; (loop < (SUMMARY_START_ADD + 256)) && (loop <= mc.mc.prod_totalcount); loop++)
		{
			for(i = 0; i < PRODUCT_SUMMARY_LEN; i++)
				g_8K_membuf[loopCount++] = temp_product_summary[loop].arr[i];
		}

		if(mc.mc.currentPG)
		{
			FlashModifyData((((unsigned long)(PRODUCT_SUMMARY_PG * PGBYTS)) +
					((unsigned long)(SUMMARY_START_ADD * PRODUCT_SUMMARY_LEN))), g_8K_membuf, loopCount);
			store_iwr(SUMMARY_START_ADD, g_8K_membuf, loopCount);
		}
		else
		{
			FlashModifyData((((unsigned long)(PRODUCT_SUMMARY1_PG * PGBYTS)) +
					((unsigned long)(SUMMARY_START_ADD * PRODUCT_SUMMARY_LEN))), g_8K_membuf, loopCount);
			store_iwr(SUMMARY_START_ADD, g_8K_membuf, loopCount);
		}

	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

		SUMMARY_START_ADD += 256;

		if(SUMMARY_START_ADD >= mc.mc.prod_totalcount)
			break;
	}
	if(mc.mc.currentPG)
		mc.mc.checkStartRecAddress = mc.mc.recs;
	else
		mc.mc.checkStartRecAddress = mc.mc.rec1;
	mc.mc.checkOffByte = SWITCH_OFF_BYTE;
	store_mc();
}
//-------------------------------------------------------------------------
unsigned long readWeighingScaleReading(void)
{
	UCHAR loopCount = 0, exit = 0, receviedChar = 0, exitMain = 0, key = 0, i = 0, buff[25], final[25];
	UCHAR signFlag = 0, valid = 0, count = 0, j = 0, dotFlag = 0;//vikas
	UINT16 buffLen = 0, finalLen = 0, dotPos = 0;
	UINT32 tempVal = 0;

	weighingScaleReceiveFlag = 0;
	weighingStartFlag = 0;

	get_ws_sett();
	while(!exitMain)
	{
		memset(asc, '\0', 20);
		memset(buff, '\0', 25);
		memset(final, '\0', 25);
		weighingStartFlag = 1;
		weighingScaleReceiveFlag = 0;
		time_out1 = 0;
		while((!weighingScaleReceiveFlag) && (time_out1 < 3000));
		if((time_out1 >= 3000) && (!weighingScaleReceiveFlag))
		{
			return 0xffffffff;
		}
		if(weighingScaleReceiveFlag)
		{
			weighingScaleReceiveFlag = 0;
			loopCount = 0;
			time_out1 = 0;
			exit = 0;
			receviedChar = 0;
			asc[loopCount++] = RXD_CHAR;
			while((!exit) && (time_out1 < 2000) && (receviedChar != '\n'))
			{
				weighingStartFlag = 0;
				receviedChar = pc_rd();
				asc[loopCount++] = receviedChar;
				if(receviedChar == '\r')
				{
					receviedChar = '\n';
				}
				else if(receviedChar == 0x04)
				{
					receviedChar = '\n';
				}
				else if(receviedChar == '}')
				{
					receviedChar = '\n';
				}
				else if(receviedChar == ')')
				{
					receviedChar = '\n';
				}
				else if(receviedChar == ']')
				{
					receviedChar = '\n';
				}
			}
		}

		signFlag = 0;
		dotFlag = 0;//vikas
		count = 0;
		valid = 0;

		if(mc.mc.weighing_scale_mode == 1)
		{
			for(i = 0; i < loopCount; i++)
			{
				if(((asc[i] >= '0') && (asc[i] <= '9')) || (asc[i] == '.'))
				{
					if(asc[i] != '.')
					{
						buff[count++] = asc[i];
					}
				}
				else
				{
					count = 0;
				}
			}

			buffLen = strlen(buff);

			if((buffLen > 0) && (buffLen <= 6))
			{
				switch(buffLen)
				{
					case 4:
							dotPos = 1;
							break;
					case 5:
							dotPos = 2;
							break;
					case 6:
							dotPos = 3;
							break;
					default:
							dotPos = (buffLen - 3);
							break;
				}

				for(i = 0, count = 0; i < buffLen; i++)
				{
					if(dotPos == i)
					{
						final[count++] = '.';
					}
					final[count++] = buff[i];
				}

				gotoxy(5,13);
				for(i = 0; i < count; i++)
				{
					disp_char(final[i]);
				}
			}
			else if(buffLen > 6)
			{
				double_height = 0;
				gotoxy(6,1);
				lcdprint("OUT OF RANGE QTY(>6)");
				beep(SHRTBEEP);
				Delay_ms(1000);
				clrRow(6);
				double_height = 1;
				return 0xffffffff;
			}
		}
		else
		{
			for(i = ws_sett.ws_sett.start; i <= ws_sett.ws_sett.end; i++)
			{
				if(((asc[i] >= '0') && (asc[i] <= '9')) || (asc[i] == ' ') || (asc[i] == '.'))
				{
					buff[count++] = asc[i];
					if(asc[i] == '.')
						dotFlag = 1;
					valid = 1;
				}
			}

			gotoxy(5,13);
			if(dotFlag)
			{
				for(i = 0; i < count; i++)
				{
					disp_char(buff[i]);
				}
			}
			else
			{
				for(i = 0; i < count; i++)
				{
					if(i == (ws_sett.ws_sett.dot_pos))
						disp_char('.');
					disp_char(buff[i]);
				}
			}
		}
		weighingStartFlag = 0;
		weighingScaleReceiveFlag = 0;

		key = keyPressCheck();

		switch (key)
		{
			case '*':
					exitMain = 1;
					return 0;
			case '#':
				    if(mc.mc.weighing_scale_mode)
				    {
				    	if(buffLen)
						{
							exitMain = 1;
							for(i = 0, j = 0; i < buffLen; i++)
							{
								if((buff[i] >= '0') && (buff[i] <= '9'))
									asc[j++] = buff[i];
								else if(buff[i] == ' ')
									asc[j++] = '0';
							}
							return asc_hex(asc, buffLen);
						}
						else
						{
							double_height = 1;
							double_width = 0;
							gotoxy(5,1);
							lcdprint("  PLEASE TARE SCALE ");
							Delay_ms(1500);
							gotoxy(5,1);
							lcdprint("    AND TRY AGAIN   ");
							beep(ERRRBEEP);
							Delay_ms(1500);
							clrRow(5);
						}
				    }
				    else
				    {
						if(count)
						{
							exitMain = 1;
							memset(asc, 0, sizeof(asc));
							for(i = 0, j = 0; i < count; i++)
							{
								if((buff[i] >= '0') && (buff[i] <= '9'))
									asc[j++] = buff[i];
								else if(buff[i] == ' ')
									asc[j++] = '0';
							}
							tempVal = asc_hex(asc, j);
							switch(ws_sett.ws_sett.digitsAfterDot)
							{
								case 0:
										tempVal = (tempVal * 1000);
										break;
								case 1:
										tempVal = (tempVal * 100);
										break;
								case 2:
										tempVal = (tempVal * 10);
										break;
								default:
										break;
							}
							return tempVal;
						}
						else
						{
							double_height = 1;
							double_width = 0;
							gotoxy(5,1);
							lcdprint("  PLEASE TARE SCALE ");
							Delay_ms(1500);
							gotoxy(5,1);
							lcdprint("    AND TRY AGAIN   ");
							beep(ERRRBEEP);
							Delay_ms(1500);
							clrRow(5);
						}
				    }

					break;
			default:
					break;
		}
	}

	weighingStartFlag = 0;
	return 0;
}
//-------------------------------------------------------------------------
void sel_func(unsigned char key)
{
	unsigned int i, ret = 0, exit = 0;
	UINT32 records = 0, lastAdd = 0, temp_qty = 0;
	static unsigned char printKeyPress = 0, shiftKeyPress = 0;

	if(barcode_entry_flag == 1)//nilesha
		key = '1';
	if(mc.mc.currentPG)
		records = mc.mc.recs;
	else
		records = mc.mc.rec1;

	if(!mc.mc.return_bill_en_dis)
		returnBill = 0;

	last_bill_amount = 0;
	spaceFlag = 0;
	calculatorMode = 0;
	searchProductSingle = 0;
	loginProcess = 0;
	switch(key)
	{
		case MENU_KEY	:
							printKeyPress = 0;
							if(loginFlag)
							{
								if(shiftFlag)
								{
									pc_comm_mode();
									shiftFlag = 0;
								}
								else
								{
									if(mc.mc.udf_en_dis && !udf_no)
										complex_manager_func();
									else if(selectedProd == 0)
										complex_manager_func();
								}
							}
							break;
		case UP_ARROW	:
							if(loginFlag)
							{
								if(shiftFlag)
								{
									shiftFlag = 0;
									printKeyPress = 0;
									edit_item_rate_only();
								}
								else
								{
									printKeyPress = 0;
									if((recArrayPtr > 0) && (selectedProd > 0))
										editOrderFromUnsavedKot();
									printKeyPress = 0;
								}
							}
							break;
		case DOWN_ARROW	:
							printKeyPress = 0;
							if(!mc.mc.rec_start)
							{
								if(mc.mc.udf_en_dis)
								{
									if(!udf_no)
										break;
								}
								if((mc.mc.return_bill_en_dis) && (selectedProd == 0) && (recArrayPtr == 0))
								{
									clrscr();
									double_height = 1;
									double_width = 0;
									gotoxy(3,1);
									lcdprint(" RETURN BILL ENABLED");
									beep(LONGBEEP);
									Delay_ms(2000);
									returnBill = 1;
									recArrayPtr = 0;
									selectedAmount = 0;
									selectedQty = 0;
									selectedProd = 0;
								}
							}
							else if((recArrayPtr > 0) && (selectedProd > 0))
							{
								editOrderFromUnsavedKot();
							}
							break;
		case '*'		:
							if(recArrayPtr > 0)
							{
								if(yes_no("    CLEAR  BILL?    "))
								{
									if(mc.mc.udf_en_dis)
										udf_no = 0;
									printKeyPress = 0;
									selectedAmount = 0;
									selectedQty = 0;
									recArrayPtr = 0;
									returnBill = 0;
									selectedProd = 0;
									returnBill = 0;
									udf_char = 0;
								}
							}
							else if(returnBill)
							{
								if(yes_no(" CANCEL RETURN BILL?"))
								{
									if(mc.mc.udf_en_dis)
										udf_no = 0;
									printKeyPress = 0;
									selectedAmount = 0;
									selectedQty = 0;
									recArrayPtr = 0;
									returnBill = 0;
									selectedProd = 0;
									returnBill = 0;
									udf_char = 0;
								}
							}
							else
							{
								if(mc.mc.udf_en_dis)
									udf_no = 0;
								printKeyPress = 0;
								selectedAmount = 0;
								selectedQty = 0;
								recArrayPtr = 0;
								returnBill = 0;
								selectedProd = 0;
								returnBill = 0;
								udf_char = 0;
							}
							printKeyPress = 0;
							shiftKeyPress = 0;
							break;
		case '#'		:
							printKeyPress = 0;
							if((loginFlag) && (!mc.mc.app_lock))
							{
								if(mc.mc.udf_en_dis)
								{
									if(!udf_no)
									{
									    if(shiftFlag)
									    {
									        KOTStatus();
									        shiftFlag = 0;
									        break;
									    }
									    else
									        break;
									}

								}

								if(!mc.mc.udf_en_dis)
									slab_no = mc.mc.default_slab;
								if(checkTempRecords(udf_no))
								{
									clrscr();
									double_height = 1;
									double_width = 0;
									gotoxy(2,1);
									lcdprint("  ALLOWED PRODUCTS  ");
									gotoxy(4,1);
									lcdprint("  PER BILL REACHED  ");
									gotoxy(6,1);
									lcdprint("  PLEASE MAKE BILL  ");
									beep(ERRRBEEP);
									Delay_ms(3000);
									break;
								}

								if(selectedProd > MAX_TEMP_RECORDS)
								{
									clrscr();
									double_height = 1;
									double_width = 0;
									gotoxy(2,1);
									gotoxy(3,1);
									lcdprint("  MAX ITEMS REACHED ");
									beep(ERRRBEEP);
									Delay_ms(3000);
									break;
								}

								if(records < (MAX_RECORD - 25))
								{
									clear_global_variable();
									ret = 0;
									searchProductSingle = 1;

									if(mc.mc.igstFlag)
									{
										if(!recArrayPtr)
										{
											flagIGST = optionGSTSelection();
										}
									}
									else
										flagIGST = 0;

									if(mc.mc.product_selection == BEFORE_QTY)
									{
										if(mc.mc.prod_totalcount <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH)
											ret = search_prd_name();
										else
											ret = searchProductByName_test(0);		//Mrugnil 27/06/24
										searchProductSingle = 0;
										if(ret == 1)
										{
											allow_get_dig_dot_shift_key = 1;
											qty = get_QUANTITY(0);
											if(qty)
											{
												allow_rate_sel = 1;
												if ((!final_bill_amount) || (mc.mc.modify_rate_enable == 1) || (mc.mc.modify_rate_enable == 2 && get_dig_dot_shift_key == 1)) //akshay290120
													enter_manual_rate(final_bill_amount);
												allow_rate_sel = 0;
												store_entry(0);
												countSI = 0;
												memset(tempArr,' ',5);
											}
										}
										else if(ret == 3)
										{
											exit = 0;
											while(!exit)
											{
												get_dig_dot_esc_key = 0;
												allow_rate_sel = 1;
												if ((!final_bill_amount) || (mc.mc.modify_rate_enable))
													enter_manual_rate(final_bill_amount);
												allow_rate_sel = 0;

												if(get_dig_dot_esc_key)
												{
													exit = 1;
													break;
												}
												get_dig_dot_esc_key = 0;
												localAmount = enter_purchase_amount();
												if(get_dig_dot_esc_key)
												{
													exit = 1;
													break;
												}
												if(localAmount)
												{
													if(final_bill_amount)
													{
														if(unit_fract)
														{
															qty = ((localAmount * 1000)/final_bill_amount);
															if(qty <= 999999)
															{
																store_entry(1);
																countSI = 0;
																memset(tempArr,' ',5);
																exit = 1;
															}
															else
															{
																countSI = 0;
																memset(tempArr,' ',5);
																clrscr();
																double_height = 1;
																gotoxy(2,1);
																lcdprint("MAX QUANTITY REACHED");
																gotoxy(4,1);
																lcdprint("ENTER SMALLER AMOUNT");
																beep(ERRRBEEP);
																Delay_ms(2000);
																clrscr();
															}
														}
														else
														{
															if((localAmount % final_bill_amount) == 0)
															{
																qty = ((localAmount * 1000)/final_bill_amount);
																if(qty <= 999000)
																{
																	store_entry(1);
																	countSI = 0;
																	memset(tempArr,' ',5);
																	exit = 1;
																}
																else
																{
																	countSI = 0;
																	memset(tempArr,' ',5);
																	clrscr();
																	double_height = 1;
																	gotoxy(2,1);
																	lcdprint("MAX QUANTITY REACHED");
																	gotoxy(4,1);
																	lcdprint("ENTER SMALLER AMOUNT");
																	beep(ERRRBEEP);
																	Delay_ms(2000);
																	clrscr();
																}
															}
															else
															{
																countSI = 0;
																memset(tempArr,' ',5);
																clrscr();
																double_height = 1;
																gotoxy(2,1);
																lcdprint("  AMOUNT SHOULD BE  ");
																gotoxy(4,1);
																lcdprint("     MULTIPLE OF    ");
																hex_asc(final_bill_amount, asc, 9);
																rem_zeros(asc, 5);
																gotoxy(6,1);
																lcdprint("RATE: ");
																for(i = 0; i < 9; i++)
																{
																	if(i == 7)
																		disp_char('.');
																	if(asc[i] != ' ')
																		disp_char(asc[i]);
																}
																beep(ERRRBEEP);
																Delay_ms(2000);
																clrscr();
															}
														}
													}
													else
													{
														countSI = 0;
														memset(tempArr,' ',5);
														clrscr();
														double_height = 1;
														gotoxy(3,1);
														lcdprint("   INVALID  ENTRY   ");
														gotoxy(5,1);
														lcdprint("  PLEASE TRY AGAIN  ");
														beep(ERRRBEEP);
														Delay_ms(2000);
														clrscr();
													}
												}
												else
												{
													countSI = 0;
													memset(tempArr,' ',5);
													clrscr();
													double_height = 1;
													gotoxy(3,1);
													lcdprint("   INVALID  ENTRY   ");
													gotoxy(5,1);
													lcdprint("  PLEASE TRY AGAIN  ");
													beep(ERRRBEEP);
													Delay_ms(2000);
													clrscr();
												}
											}
										}
										else if(ret == 0)
										{
											countSI = 0;
											memset(tempArr,' ',5);
											clrscr();
										}
									}
									else
									{
										qty = get_BLANK_QUANTITY(asc_hex(tempArr, countSI));

										if(qty)
										{
											if(mc.mc.prod_totalcount <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH)
												ret = search_prd_name();
											else
												ret = searchProductByName_test(0);		//Mrugnil 27/06/24
											searchProductSingle = 0;

											if(ret)
											{
												if(!unit_fract)
												{
													hex_asc(qty, asc, 6);
													if(mc.mc.qty_enter_method == QTY_3_3)
													{
														temp_qty = asc_hex(asc, 3);
														qty = temp_qty * 1000;
													}
													else
													{
														temp_qty = asc_hex(asc, 4);
														qty = temp_qty * 100;
													}
												}
												allow_rate_sel = 1;
												if ((!final_bill_amount) ||(mc.mc.modify_rate_enable))
													enter_manual_rate(final_bill_amount);
												allow_rate_sel = 0;

												store_entry(0);
											}
											countSI = 0;
											memset(tempArr,' ',5);
										}
									}
									countSI = 0;
									memset(tempArr,' ',5);
								}
								else
								{
									clrscr();
									double_height = 1;
									gotoxy(3,1);
									lcdprint("    MEMORY  FULL    ");
									gotoxy(5,1);
									lcdprint("   ERASE  RECORDS   ");
									beep(ERRRBEEP);
									Delay_ms(2000);
									clrscr();
								}
							}
							if(mc.mc.app_lock)
							{
								clrscr();
								double_height = 1;
								gotoxy(3, 1);
								lcdprint("   PLEASE CONTACT   ");
								gotoxy(5, 1);
								lcdprint("  SERVICE PROVIDER  ");
								beep(ERRRBEEP);
								Delay_ms(3000);
							}
							break;
		case PRINT_KEY	:
							if((!printKeyPress) && (!shiftKeyPress))		//Mrugnil 06/04/24
							{
								keyPressTimeOut = 0;
								printKeyPress = 1;		//Mrugnil 06/04/24
							}
							else if((!printKeyPress) && (shiftKeyPress))		//Mrugnil 06/04/24
							{
								if((keyPressTimeOut < 1000) && (keyPressTimeOut > 0))		//Mrugnil 06/04/24
								{
									keyPressTimeOut = 0;
									printKeyPress = 1;		//Mrugnil 06/04/24
									shiftKeyPress = 1;		//Mrugnil 06/04/24
								}
								else
								{
									printKeyPress = 1;		//Mrugnil 06/04/24
									shiftKeyPress = 0;		//Mrugnil 06/04/24
									keyPressTimeOut = 0;		//Mrugnil 06/04/24
								}
							}
							else if(printKeyPress)		//Mrugnil 06/04/24
							{
								if((keyPressTimeOut < 1000) && (keyPressTimeOut > 0))
								{
									if(mc.mc.drawer_en_dis)		//Mrugnil 06/04/24
									{
										if(yes_no("  OPEN CASH DRAWER? "))		//Mrugnil 06/04/24
										{
											if(mc.mc.drawer_en_dis)		//Mrugnil 06/04/24
											{
												DRW_ENABLE = 1;		//Mrugnil 06/04/24
												Delay_ms(200);		//Mrugnil 06/04/24
												DRW_ENABLE = 0;		//Mrugnil 06/04/24
											}
										}
									}
									printKeyPress = 0;		//Mrugnil 06/04/24
									shiftKeyPress = 0;		//Mrugnil 06/04/24
								}
								if((shiftKeyPress) && (printKeyPress))		//Mrugnil 06/04/24
								{
									printKeyPress = 1;		//Mrugnil 06/04/24
									shiftKeyPress = 1;		//Mrugnil 06/04/24
								}
								else if((!shiftKeyPress) && (printKeyPress))		//Mrugnil 06/04/24
								{
									printKeyPress = 0;		//Mrugnil 06/04/24
									shiftKeyPress = 0;		//Mrugnil 06/04/24
								}
							}

//							if((shiftFlag) && (printKeyPress))
							if((shiftKeyPress) && (printKeyPress))		//Mrugnil 06/04/24
							{
								paper_feed();		//Mrugnil 06/04/24
//								shiftFlag = 0;
								shiftKeyPress = 0;		//Mrugnil 06/04/24
								printKeyPress = 0;
							}
							else
							{
								if(loginFlag && (!mc.mc.app_lock))
								{
									if(mc.mc.udf_en_dis)
									{
										if(!udf_no)
										{
											break;
										}
										if(udf_no && (!selectedProd))
										{
											get_udf(udf_no);
											if(!udfno.udfno.udfActive)
											{
												break;
											}
										}
									}
									else
									{
										if(!selectedProd)
										{
											break;
										}
									}

									clear_global_variable();
									print_mode = printType();

									switch(print_mode)
									{
										case 1:
												//print KOT
												saveOrder();
												if(selectedProd > 0)
													KOTprint();
												recArrayPtr = 0;
												udf_no = 0;
												edit_kot_qty_selected = 0;
												edit_kot_record_start = 0;//vikas
												edit_kot_record_end = 0;
												edit_kot_qty_flag = 0;
												break;
										case 2:
												//print BILL
												if(mc.mc.paymode && (!returnBill))
												{
													paymentType = paymentMode();
													if(paymentType == 0xFF)		//Mrugnil 10/09/24
														break;		//Mrugnil 10/09/24
												}
												else
												{
													paymentType = 1;
												}
												if(selectedProd > 0)
													saveOrder();

												if(mc.mc.cust_name_entry)
												{
													if((mc.mc.cust_name_entry == CUST_BOTH)
															|| (mc.mc.cust_name_entry == O_CUST_NAME))
													{
														clrscr();
														double_height = 1;
														double_width = 0;
														gotoxy(1,1);
														lcdprint("ENTER CUSTOMER NAME ");
														double_height = 0;
														gotoxy(8,1);
														lcdprint("PRESS        ENT/CNL");
														double_height = 1;

														getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
													}

													if((mc.mc.cust_name_entry == CUST_BOTH)
															|| (mc.mc.cust_name_entry == O_GSTIN))
													{
														clrscr();
														double_height = 1;
														double_width = 0;
														gotoxy(1,1);
														lcdprint("ENTER GSTIN NO:   ");
														double_height = 0;
														gotoxy(8,1);
														lcdprint("PRESS        ENT/CNL");
														double_height = 1;

														getAlphaDig(4, 1, g_GST_IN, GSTIN_NO_LEN, 1, 0);
													}
												}

												if(store_last_rec())
												{
													recArrayPtr = 0;
													loadOrderDetailsIntoRAM();

													if(mc.mc.currentPG)
														lastAdd = mc.mc.recs;
													else
														lastAdd = mc.mc.rec1;

													closeOpenBill(startRec, lastAdd, 0);

													if(mc.mc.cash_reception_enable && (!returnBill))
													{
														if(rec.rec.payment_mode == 1)
														{
															while(!exit)
															{
																clrscr();
																double_height = 0;
																gotoxy(1,1);
																lcdprint("   CASH RECEPTION   ");

																gotoxy(8,1);
																lcdprint("             CNL/ENT");
																double_height = 1;
																gotoxy(3,1);
																lcdprint("BILL AMT: ");
																hex_asc(rec.rec.round_off_tot_amt, asc, 9);
																rem_zeros(asc, 5);
																for(i = 0; i < 9; i++)
																{
																	if(i == 7)
																		disp_char('.');
																	disp_char(asc[i]);
																}																
																gotoxy(6,1);
																lcdprint("PAY AMT: ");																
																recordedDotPosition = 0;
																get_dig_dot_esc_key = 0;
																recevied_cash = get_dig_dot(6,10,7,2,0,0);
																if(get_dig_dot_esc_key)
																{
																	recevied_cash = 0;
																	returned_amount = 0;
																	credit_amount = rec.rec.round_off_tot_amt;
																	exit = 1;
																}
																else
																{
																	if(recevied_cash < rec.rec.round_off_tot_amt)
																	{
																		credit_amount = (rec.rec.round_off_tot_amt - recevied_cash);
																		exit = 1;
																	}
																	else
																	{
																		returned_amount = (recevied_cash - rec.rec.round_off_tot_amt);
																		exit = 1;
																	}
																}
															}
														}
													}
													if(mc.mc.udf_en_dis)
													{
														get_udf(udf_no);
														udfno.udfno.udfActive = 0;
														udfno.udfno.udfStart = 0;
														udfno.udfno.udfRcptNo = 0;
														store_udf(udf_no);

														get_udf_prd_cnt(udf_no);
														udf_prd_cnt.udf_prd_cnt.prd_count = 0;
														store_udf_prd_cnt(udf_no);
													}
													if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
													{
														if(mc.mc.rcpt_no >= MAX_RCPTNO)
															mc.mc.rcpt_no = 1;
														else
															mc.mc.rcpt_no++;
													}
													store_mc();

													if(mc.mc.udf_en_dis)
														bill_print();
													else
													{
														if(mc.mc.bill_format_selection == TAX_SORTING)
														{
															printNonUDFBill();

////															if((mc.mc.coupon_en_dis) && (!mc.mc.udf_en_dis) && (!mc.mc.kot_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//															if((mc.mc.coupon_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//															{
//																if(yes_no("   PRINT  COUPON?   "))
//																	couponprint();
//															}
															if((mc.mc.coupon_en_dis) && (!returnBill))		//Mrugnil 30/01/24
															{
																if(yes_no("   PRINT  COUPON?   "))		//Mrugnil 30/01/24
																	couponprint();
															}
														}
														else
															bill_print();
													}

													if(mc.mc.drawer_en_dis) //VIKAS
													{
														DRW_ENABLE = 1;
														Delay_ms(200);
														DRW_ENABLE = 0;
													}

													mc.mc.rec_start = 0;
													mc.mc.selected = 0;
													store_mc();
													selectedAmount = 0;
													selectedQty = 0;
													selectedProd = 0;
													recArrayPtr = 0;
													returnBill = 0;

													if(mc.mc.gprs_en_dis)
													{
														stopPostCMDToModem();
														pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
													}

													if(mc.mc.gsm_en_dis == RECORD_LOT)
													{
														if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
														{
															sendDayWiseSummary_SMSToAllMobileNumbers();
														}
													}
													udf_no = 0;
													udf_char = 0;
												}
												else
												{
													clrscr();
													double_height = 1;
													double_width = 0;
													gotoxy(3,1);
													lcdprint("   NO BILL FOUND!   ");
													beep(ERRRBEEP);
													Delay_ms(3000);
													break;
												}
												break;
										case 4:
												//edit KOT
												if((recArrayPtr > 0) && (selectedProd > 0))
												{
													editOrderFromUnsavedKot();
												}
												else
													editOrder();
												if((recArrayPtr > 0) && (selectedProd > 0))
												{
													if(yes_no("    SAVE & PRINT?   "))
													{
														saveOrder();
														KOTprint();
														selectedAmount = 0;
														selectedQty = 0;
														selectedProd = 0;
														recArrayPtr = 0;
														udf_no = 0;
														edit_kot_qty_selected = 0;
														edit_kot_record_start = 0;//vikas
														edit_kot_record_end = 0;
														edit_kot_qty_flag = 0;
													}
												}
												else if((edit_kot_qty_selected > 0))
												{
													if(yes_no("  EDITED KOT PRINT? "))
													{
														edit_kot_qty_flag = 1;//vikas
//														EditedKOTprint();
														KOTprint();
														selectedAmount = 0;
														selectedQty = 0;
														selectedProd = 0;
														recArrayPtr = 0;
														udf_no = 0;
														edit_kot_qty_selected = 0;
														edit_kot_record_start = 0;
														edit_kot_record_end = 0;
														edit_kot_qty_flag = 0;
													}
												}
												break;
										case 3:
												if(selectedProd)
												{
													saveOrder();
													clrscr();
													double_height = 1;
													double_width = 0;
													gotoxy(3,1);
													lcdprint("     KOT  SAVED     ");
													beep(LONGBEEP);
													Delay_ms(1000);
													recArrayPtr = 0;
													udf_no = 0;
												}
												break;
										case 5:
												viewBillAmount(udf_no);
												break;
										case 6:
												//print BILL
												if(mc.mc.paymode && (!returnBill))
												{
													paymentType = paymentMode();
												}
												else
												{
													paymentType = 1;
												}
												if(selectedProd > 0)
													saveOrder();
												if(mc.mc.cust_name_entry)
												{
													if((mc.mc.cust_name_entry == CUST_BOTH)
															|| (mc.mc.cust_name_entry == O_CUST_NAME))
													{
														clrscr();
														double_height = 1;
														double_width = 0;
														gotoxy(1,1);
														lcdprint("ENTER CUSTOMER NAME ");
														double_height = 0;
														gotoxy(8,1);
														lcdprint("PRESS        ENT/CNL");
														double_height = 1;

														getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
													}

													if((mc.mc.cust_name_entry == CUST_BOTH)
															|| (mc.mc.cust_name_entry == O_GSTIN))
													{
														clrscr();
														double_height = 1;
														double_width = 0;
														gotoxy(1,1);
														lcdprint("ENTER GSTIN NO:   ");
														double_height = 0;
														gotoxy(8,1);
														lcdprint("PRESS        ENT/CNL");
														double_height = 1;

														getAlphaDig(4, 1, g_GST_IN, GSTIN_NO_LEN, 1, 0);
													}
												}
												if(store_last_rec())
												{
													if(mc.mc.currentPG)
														records = mc.mc.recs;
													else
														records = mc.mc.rec1;
													closeOpenBill(startRec, records, 0);

													if(mc.mc.udf_en_dis)
													{
														get_udf(udf_no);
														udfno.udfno.udfActive = 0;
														udfno.udfno.udfStart = 0;
														udfno.udfno.udfRcptNo = 0;
														store_udf(udf_no);

														get_udf_prd_cnt(udf_no);
														udf_prd_cnt.udf_prd_cnt.prd_count = 0;
														store_udf_prd_cnt(udf_no);
													}

////													if((mc.mc.coupon_en_dis) && (!mc.mc.udf_en_dis) && (!mc.mc.kot_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//													if((mc.mc.coupon_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//													{
//														if(yes_no("   PRINT  COUPON?   "))
//															couponprint();
//													}
													if((mc.mc.coupon_en_dis) && (!returnBill))		//Mrugnil 30/01/24
													{
														if(yes_no("   PRINT  COUPON?   "))		//Mrugnil 30/01/24
															couponprint();
													}

													if(mc.mc.drawer_en_dis)
													{
														DRW_ENABLE = 1;
														Delay_ms(200);
														DRW_ENABLE = 0;
													}

													mc.mc.rec_start = 0;
													mc.mc.selected = 0;
													selectedAmount = 0;
													selectedQty = 0;
													selectedProd = 0;
													recArrayPtr = 0;
													returnBill = 0;
													if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
													{
														if(mc.mc.rcpt_no >= MAX_RCPTNO)
															mc.mc.rcpt_no = 1;
														else
															mc.mc.rcpt_no++;
													}
													store_mc();
													clrscr();
													double_height = 1;
													double_width = 0;
													gotoxy(3,1);
													lcdprint("     BILL SAVED     ");
													Delay_ms(1000);

													if(mc.mc.gprs_en_dis)
													{
														stopPostCMDToModem();
														pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
													}

													if(mc.mc.gsm_en_dis == RECORD_LOT)
													{
														if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
														{
															sendDayWiseSummary_SMSToAllMobileNumbers();
														}
													}
													udf_no = 0;
												}
												break;
										default:
												break;
									}
								}
								if(mc.mc.app_lock)
								{
									clrscr();
									double_height = 1;
									gotoxy(3, 1);
									lcdprint("   PLEASE CONTACT   ");
									gotoxy(5, 1);
									lcdprint("  SERVICE PROVIDER  ");
									beep(ERRRBEEP);
									Delay_ms(3000);
								}
							}
							break;
		case SHIFT_KEY:
						printKeyPress = 0;
						shiftFlag = ~shiftFlag;
						if(!shiftKeyPress)
						{
							keyPressTimeOut = 0;
							shiftKeyPress = 1;
						}
						else
						{
							if(mc.mc.udf_en_dis)
							{
								if(udf_no)
									break;
							}
							else
							{
								if(selectedProd || recArrayPtr)
									break;
							}
							if((keyPressTimeOut < 1000) && (keyPressTimeOut > 0))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(3,1);
								lcdprint(" DUPLICATE RCPT MODE");
								beep(SHRTBEEP);
								Delay_ms(2000);
								print_dup_rcpt_multiple();
								recArrayPtr = 0;
								shiftKeyPress = 0;
							}
							else
								shiftKeyPress = 0;
						}
						break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
							printKeyPress = 0;
							if(loginFlag && (!mc.mc.app_lock))
							{
								if((!udf_no) && (mc.mc.udf_en_dis))
								{
									if((key >= '1') && ( key <= '9'))
									{
										udf_no = udf_entry(key);
										if(udf_no > 0)
										{
											recArrayPtr = 0;
											selectedProd = 0;
											startProd = 0;
											selectedAmount = 0;
											selectedQty = 0;
											get_udf(udf_no);
											if(udfno.udfno.udfActive)
											{
												loadOrderDetailsIntoRAM();
											}
											recArrayPtr = 0;
											selectedProd = 0;
											startProd = 0;
											slab_no = 1;
											if(mc.mc.udf_en_dis)
											{
												if(mc.mc.slab_totalcount)
												{
													for(i = 0; i < mc.mc.slab_totalcount; i++)
													{
														get_rate_slab(i);
														if((slab.slab.frm_table <= udf_no) && (slab.slab.to_table >= udf_no))
															slab_no = slab.slab.rate_slab;
													}
												}
												else
													slab_no = 1;
											}
											else
												slab_no = 1;
										}
									}
								}
								else
								{
									if(checkTempRecords(udf_no))
									{
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(2,1);
										lcdprint("  ALLOWED PRODUCTS  ");
										gotoxy(4,1);
										lcdprint("  PER BILL REACHED  ");
										gotoxy(6,1);
										lcdprint("  PLEASE MAKE BILL  ");
										beep(ERRRBEEP);
										Delay_ms(3000);
										break;
									}

									if(selectedProd > MAX_TEMP_RECORDS)
									{
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("  MAX ITEMS REACHED ");
										beep(ERRRBEEP);
										Delay_ms(3000);
										break;
									}

									if(!mc.mc.udf_en_dis)
										slab_no = mc.mc.default_slab;
									if(records < (MAX_RECORD - 25))
									{
										if((key >= '1') && ( key <= '9'))
										{
											clear_global_variable();
											if(countSI < 4)
												tempArr[countSI++] = key;
											ret = 0;
											if(mc.mc.igstFlag)
											{
												if(!recArrayPtr)
												{
													flagIGST = optionGSTSelection();
												}
											}
											else
												flagIGST = 0;

											searchProductSingle = 1;
											if(mc.mc.product_selection == BEFORE_QTY)
											{
												if(barcode_entry_flag == 1)
												{
													ret = searchProduct(0, 0);		//Mrugnil 27/06/24
												}
												else
													ret = searchProduct(asc_hex(tempArr, countSI), 0);		//Mrugnil 27/06/24
												searchProductSingle = 0;
												if(ret == 1)
												{
													allow_get_dig_dot_shift_key = 1;
													insideQuantityFunction = 1;		//Mrugnil 31/01/24
													qty = get_QUANTITY(0);
													insideQuantityFunction = 0;		//Mrugnil 31/01/24
													if(qty)
													{
														allow_rate_sel = 1;
															if ((!final_bill_amount) ||(mc.mc.modify_rate_enable == 1) || (mc.mc.modify_rate_enable == 2 && get_dig_dot_shift_key == 1)) //akshay290120
															enter_manual_rate(final_bill_amount);
														allow_rate_sel = 0;
														store_entry(0);
														countSI = 0;
														memset(tempArr,' ',5);
													}
												}
												else if(ret == 3)
												{
													exit = 0;
													while(!exit)
													{
														get_dig_dot_esc_key = 0;
														allow_rate_sel = 1;
														if ((!final_bill_amount) ||(mc.mc.modify_rate_enable))
															enter_manual_rate(final_bill_amount);
														allow_rate_sel = 0;

														if(get_dig_dot_esc_key)
														{
															exit = 1;
															break;
														}
														get_dig_dot_esc_key = 0;
														localAmount = enter_purchase_amount();
														if(get_dig_dot_esc_key)
														{
															exit = 1;
															break;
														}
														if(localAmount)
														{
															if(final_bill_amount)
															{
																if(unit_fract)
																{
																	qty = ((localAmount * 1000)/final_bill_amount);
																	if(qty <= 999999)
																	{
																		store_entry(1);
																		countSI = 0;
																		memset(tempArr,' ',5);
																		exit = 1;
																	}
																	else
																	{
																		countSI = 0;
																		memset(tempArr,' ',5);
																		clrscr();
																		double_height = 1;
																		gotoxy(2,1);
																		lcdprint("MAX QUANTITY REACHED");
																		gotoxy(4,1);
																		lcdprint("ENTER SMALLER AMOUNT");
																		beep(ERRRBEEP);
																		Delay_ms(2000);
																		clrscr();
																	}
																}
																else
																{
																	if((localAmount % final_bill_amount) == 0)
																	{
																		qty = ((localAmount * 1000)/final_bill_amount);
																		if(qty <= 999000)
																		{
																			store_entry(1);
																			countSI = 0;
																			memset(tempArr,' ',5);
																			exit = 1;
																		}
																		else
																		{
																			countSI = 0;
																			memset(tempArr,' ',5);
																			clrscr();
																			double_height = 1;
																			gotoxy(2,1);
																			lcdprint("MAX QUANTITY REACHED");
																			gotoxy(4,1);
																			lcdprint("ENTER SMALLER AMOUNT");
																			beep(ERRRBEEP);
																			Delay_ms(2000);
																			clrscr();
																		}
																	}
																	else
																	{
																		countSI = 0;
																		memset(tempArr,' ',5);
																		clrscr();
																		double_height = 1;
																		gotoxy(2,1);
																		lcdprint("  AMOUNT SHOULD BE  ");
																		gotoxy(4,1);
																		lcdprint("     MULTIPLE OF    ");
																		hex_asc(final_bill_amount, asc, 9);
																		rem_zeros(asc, 5);
																		gotoxy(6,1);
																		lcdprint("RATE: ");
																		for(i = 0; i < 9; i++)
																		{
																			if(i == 7)
																				disp_char('.');
																			if(asc[i] != ' ')
																				disp_char(asc[i]);
																		}
																		beep(ERRRBEEP);
																		Delay_ms(2000);
																		clrscr();
																	}
																}
															}
															else
															{
																countSI = 0;
																memset(tempArr,' ',5);
																clrscr();
																double_height = 1;
																gotoxy(3,1);
																lcdprint("   INVALID  ENTRY   ");
																gotoxy(5,1);
																lcdprint("  PLEASE TRY AGAIN  ");
																beep(ERRRBEEP);
																Delay_ms(2000);
																clrscr();
															}
														}
														else
														{
															countSI = 0;
															memset(tempArr,' ',5);
															clrscr();
															double_height = 1;
															gotoxy(3,1);
															lcdprint("   INVALID  ENTRY   ");
															gotoxy(5,1);
															lcdprint("  PLEASE TRY AGAIN  ");
															beep(ERRRBEEP);
															Delay_ms(2000);
															clrscr();
														}
													}
												}
												else if(ret == 0)
												{
													countSI = 0;
													memset(tempArr,' ',5);
													clrscr();
												}
												else if(ret == 4) //vikas
												{
													qty = 1000;
													if(qty)
													{
														allow_rate_sel = 1;
														if ((!final_bill_amount) ||(mc.mc.modify_rate_enable))
															enter_manual_rate(final_bill_amount);
														allow_rate_sel = 0;

														store_entry(0);
														countSI = 0;
														memset(tempArr,' ',5);
													}
												}
											}
											else
											{
												if(!recArrayPtr)
												{
													countSI = 1;
													tempArr[0] = '0';
												}
												qty = get_BLANK_QUANTITY(asc_hex(tempArr, countSI));

												memset(tempArr,' ',5);
												countSI = 1;
												tempArr[0] = '1';
												if(qty)
												{
													if(barcode_entry_flag == 1)
													{
														ret = searchProduct(0, 0);		//Mrugnil 27/06/24
													}
													else
														ret = searchProduct(1, 0);		//Mrugnil 27/06/24
													searchProductSingle = 0;
													if(ret)
													{
														if(!unit_fract)
														{
															hex_asc(qty, asc, 6);
															if(mc.mc.qty_enter_method == QTY_3_3)
															{
																temp_qty = asc_hex(asc, 3);
																qty = temp_qty * 1000;
															}
															else
															{
																temp_qty = asc_hex(asc, 4);
																qty = temp_qty * 100;
															}
														}
														allow_rate_sel = 1;
														if ((!final_bill_amount) ||(mc.mc.modify_rate_enable))
															enter_manual_rate(final_bill_amount);
														allow_rate_sel = 0;

														store_entry(0);
													}
													countSI = 0;
													memset(tempArr,' ',5);
												}
											}

											countSI = 0;
											memset(tempArr,' ',5);
										}
									}
									else
									{
										clrscr();
										double_height = 1;
										gotoxy(3,1);
										lcdprint("    MEMORY  FULL    ");
										gotoxy(5,1);
										lcdprint("   ERASE  RECORDS   ");
										beep(ERRRBEEP);
										Delay_ms(2000);
										clrscr();
									}
								}
							}
							if(mc.mc.app_lock)
							{
								clrscr();
								double_height = 1;
								gotoxy(3, 1);
								lcdprint("   PLEASE CONTACT   ");
								gotoxy(5, 1);
								lcdprint("  SERVICE PROVIDER  ");
								beep(ERRRBEEP);
								Delay_ms(3000);
							}
							break;
		case '0':
							if(loginFlag && (!mc.mc.app_lock))
							{
								if(records < (MAX_RECORD - 25))
								{
									if(mc.mc.udf_en_dis)
									{
										if(!udf_no)
										{
											if(mc.mc.calculator_swap)
												calculator_mode();
											else
												calculator_mode_swapped();
										}
									}
									else
									{
										if(!recArrayPtr)
										{
											if(mc.mc.calculator_swap)
												calculator_mode();
											else
												calculator_mode_swapped();
										}
									}
								}
								else
								{
									clrscr();
									double_height = 1;
									gotoxy(3,1);
									lcdprint("    MEMORY  FULL    ");
									gotoxy(5,1);
									lcdprint("   ERASE  RECORDS   ");
									beep(ERRRBEEP);
									Delay_ms(2000);
									clrscr();
								}
							}
							if(mc.mc.app_lock)
							{
								clrscr();
								double_height = 1;
								gotoxy(3, 1);
								lcdprint("   PLEASE CONTACT   ");
								gotoxy(5, 1);
								lcdprint("  SERVICE PROVIDER  ");
								beep(ERRRBEEP);
								Delay_ms(3000);
							}
							break;
		default:
							printKeyPress = 0;
							if(loginFlag && (!mc.mc.app_lock))
							{
								if(records < (MAX_RECORD - 25))
								{
									if(mc.mc.udf_en_dis)
									{
										if(!udf_no)
											break;
									}
									if(!mc.mc.udf_en_dis)
										slab_no = mc.mc.default_slab;

									if(checkTempRecords(udf_no))
									{
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(2,1);
										lcdprint("  ALLOWED PRODUCTS  ");
										gotoxy(4,1);
										lcdprint("  PER BILL REACHED  ");
										gotoxy(6,1);
										lcdprint("  PLEASE MAKE BILL  ");
										beep(ERRRBEEP);
										Delay_ms(3000);
										break;
									}

									if(selectedProd > MAX_TEMP_RECORDS)
									{
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("  MAX ITEMS REACHED ");
										beep(ERRRBEEP);
										Delay_ms(3000);
										break;
									}
									clear_global_variable();

									if(mc.mc.igstFlag)
									{
										if(!recArrayPtr)
										{
											flagIGST = optionGSTSelection();
										}
									}
									else
										flagIGST = 0;

									if(mc.mc.product_selection == BEFORE_QTY)
									{
										product_code = hot_key_prod(key);

										if((product_code > 0) && (product_code != 65535))
										{
											if(product_code)
											{
												ret = 0;
												searchProductSingle = 1;
												ret = getHotKeyProductDetails(product_code);
												searchProductSingle = 0;
												if(ret == 1)
												{
													allow_get_dig_dot_shift_key = 1;
													qty = get_QUANTITY(0);
													if(qty)
													{
														allow_rate_sel = 1;
														//if(mc.mc.modify_rate_enable)
														//if ((mc.mc.modify_rate_enable == 1) || (mc.mc.modify_rate_enable == 2 && get_dig_dot_shift_key == 1)) //akshay290120
														if ((!final_bill_amount) ||(mc.mc.modify_rate_enable == 1) || (mc.mc.modify_rate_enable == 2 && get_dig_dot_shift_key == 1)) //akshay290120
															enter_manual_rate(final_bill_amount);
														allow_rate_sel = 0;
														store_entry(0);
													}
												}
												else if(ret == 2)
												{
													countSI = 0;
													clrscr();
													double_height = 1;
													gotoxy(3,1);
													lcdprint("   INVALID  ENTRY   ");
													gotoxy(5,1);
													lcdprint("  PLEASE TRY AGAIN  ");
													beep(ERRRBEEP);
													Delay_ms(2000);
													clrscr();
												}
											}
										}
										else
										{
											clrscr();
											double_height = 1;
											gotoxy(3,1);
											lcdprint("   HOT KEY NOT SET  ");
											beep(ERRRBEEP);
											Delay_ms(2000);
											clrscr();
										}
									}
									else
									{
										qty = get_BLANK_QUANTITY(asc_hex(tempArr, countSI));

										if(qty)
										{
											product_code = hot_key_prod('1');

											if((product_code > 0) && (product_code != 65535))
											{
												if(product_code)
												{
													ret = 0;
													searchProductSingle = 1;
													ret = getHotKeyProductDetails(product_code);
													searchProductSingle = 0;
													if(!unit_fract)
													{
														hex_asc(qty, asc, 6);
														if(mc.mc.qty_enter_method == QTY_3_3)
														{
															temp_qty = asc_hex(asc, 3);
															qty = temp_qty * 1000;
														}
														else
														{
															temp_qty = asc_hex(asc, 4);
															qty = temp_qty * 100;
														}
													}
													allow_rate_sel = 1;
													if ((!final_bill_amount) ||(mc.mc.modify_rate_enable))
														enter_manual_rate(final_bill_amount);
													allow_rate_sel = 0;
												}
											}
											else
											{
												clrscr();
												double_height = 1;
												gotoxy(3,1);
												lcdprint("   HOT KEY NOT SET  ");
												beep(ERRRBEEP);
												Delay_ms(2000);
												clrscr();
											}

											store_entry(0);
											countSI = 0;
											memset(tempArr,' ',5);
										}
									}
								}
								else
								{
									clrscr();
									double_height = 1;
									gotoxy(3,1);
									lcdprint("    MEMORY  FULL    ");
									gotoxy(5,1);
									lcdprint("   ERASE  RECORDS   ");
									beep(ERRRBEEP);
									Delay_ms(2000);
									clrscr();
								}
							}
							if(mc.mc.app_lock)
							{
								clrscr();
								double_height = 1;
								gotoxy(3, 1);
								lcdprint("   PLEASE CONTACT   ");
								gotoxy(5, 1);
								lcdprint("  SERVICE PROVIDER  ");
								beep(ERRRBEEP);
								Delay_ms(3000);
							}
							break;
	}

	clrscr();
}
//-------------------------------------------------------------------------
unsigned char checkTempRecords(unsigned int udf)
{
	if(!mc.mc.udf_en_dis)
		return 0;
	get_udf(udf);

	if(udfno.udfno.udfActive)
	{
		get_udf_prd_cnt(udf);
		if((udf_prd_cnt.udf_prd_cnt.prd_count + selectedProd) <= MAX_TEMP_RECORDS)
		{
			return 0;
		}
	}
	else
		return 0;
	return 1;
}
//-------------------------------------------------------------------------
//unsigned char returnBillFunction(void)
//{
//	UCHAR pageFlag = 0, found = 0;
//	UINT32 recs = 0, loopCount = 0;
//	UINT32 receiptNo = 0;
//	UINT16 i = 0;
//
//	returnDay = 0;
//	returnPageFlag = 0;
//
////	if(mc.mc.billNoType)
////	{
////		returnDay = day_select();
////
////		if(!returnDay)
////			return 3;
////
////		if(returnDay == 1)
////		{
////			if(mc.mc.currentPG)
////				recs = mc.mc.recs;
////			else
////				recs = mc.mc.rec1;
////			returnPageFlag = mc.mc.currentPG;
////		}
////		else
////		{
////			if(mc.mc.currentPG)
////			{
////				recs = mc.mc.rec1;
////				returnPageFlag = 0;
////			}
////			else
////			{
////				recs = mc.mc.recs;
////				returnPageFlag = 1;
////			}
////		}
////	}
//
//	if(mc.mc.currentPG)
//		recs = mc.mc.recs;
//	else
//		recs = mc.mc.rec1;
//	returnPageFlag = mc.mc.currentPG;
//	returnDay = 1;
//
//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(1,1);
//	lcdprint("ENTER RECEIPT :     ");
//	double_height = 0;
//	gotoxy(8,1);
//	lcdprint("PRESS        ENT/CNL");
//	double_height = 1;
//
//	receiptNo = get_dig(5,13,5,0,1,0);
//
//	returnBillReceiptNo = 0;
//
//	if(receiptNo)
//		returnBillReceiptNo = receiptNo;
//
//	clear_global_variable();
////	if(receiptNo && returnDay)
////	{
//		recArrayPtr = 0;
//		for(loopCount = 0; loopCount < recs; loopCount++)
//		{
//			get_rec_report(loopCount, returnPageFlag);
//
//			if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last != 1))
//			{
//				if((rec.rec.strt_byt != 'D'))
//				{
//					for(i = 0; i < RECDAT_LEN; i++)
//						recArray[recArrayPtr][i] = rec.arr[i];
//					recArrayPtr++;
//					found = 1;
//				}
//			}
//			else if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last == 1))
//			{
//				paymentType = rec.rec.payment_mode;
//				flagIGST = rec.rec.igstFlag;
//			}
//		}
////	}
////	else if(receiptNo && (!returnDay))
////	{
////		recArrayPtr = 0;
////		for(loopCount = 0; loopCount < mc.mc.recs; loopCount++)
////		{
////			get_rec_report(loopCount, 1);
////			returnPageFlag = 1;
////			if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last != 1))
////			{
////				if(rec.rec.strt_byt != 'D')
////				{
////					for(i = 0; i < RECDAT_LEN; i++)
////						recArray[recArrayPtr][i] = rec.arr[i];
////					recArrayPtr++;
////					found = 1;
////
////					if(mc.mc.currentPG == 1)
////					{
////						returnDay = 1;
////					}
////					else
////					{
////						returnDay = 2;
////					}
////				}
////			}
////			else if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last == 1))
////			{
////				paymentType = rec.rec.payment_mode;
////			}
////
////		}
////
////		if(!found)
////		{
////			recArrayPtr = 0;
////			for(loopCount = 0; loopCount < mc.mc.rec1; loopCount++)
////			{
////				get_rec_report(loopCount, 0);
////				returnPageFlag = 0;
////
////				if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last != 1))
////				{
////					if(rec.rec.strt_byt != 'D')
////					{
////						for(i = 0; i < RECDAT_LEN; i++)
////							recArray[recArrayPtr][i] = rec.arr[i];
////						recArrayPtr++;
////						found = 1;
////						if(mc.mc.currentPG == 0)
////						{
////							returnDay = 1;
////						}
////						else
////						{
////							returnDay = 2;
////						}
////					}
////				}
////				else if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat == 0) && (rec.rec.last == 1))
////				{
////					paymentType = rec.rec.payment_mode;
////				}
////			}
////		}
////	}
//
//	if(found)
//		showBill(receiptNo);
//	else if(returnBillReceiptNo && (!found))
//	{
//		clrscr();
//		double_height = 1;
//		double_width = 0;
//		gotoxy(3,1);
//		lcdprint("   BILL NOT FOUND   ");
//		gotoxy(5,1);
//		lcdprint(" RETURN BILL ENABLED");
//		beep(LONGBEEP);
//		Delay_ms(2000);
//		return 1;
//	}
//
//	return 0;
//
//}
////-------------------------------------------------------------------------
//void showBill(unsigned long recptNo)
//{
//	UCHAR i = 0, exit = 0, key = 0, scrollRecord = 0, found = 0;
//	UINT64 tempvar64U = 0, tempAmount = 0;
//	UINT32 tempQty = 0;
//	SINT16 j = 0;
//
//	g_returnCount = 0;
//	g_actualCount = 0;
//
//	while(!exit)
//	{
//		clrscr();
//		double_height = 1;
//		double_width = 0;
//		gotoxy(1,1);
//		lcdprint("  RETURN BILL MENU  ");
//		double_height = 0;
//		for(i = 0; i < RECDAT_LEN; i++)
//			temprec.arr[i] = recArray[scrollRecord][i];
//		hex_asc(temprec.temprec.rcpt_no, asc, 5);
//		rem_zeros(asc, 4);
//		gotoxy(3,1);
//		lcdprint("BILL NO : ");
//		for(i = 0; i < 5; i++)
//			disp_char(asc[i]);
//
//		FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//
//		if((membuf[PRODUCT_EN_FLAG] == ',') && (temprec.temprec.returnedProduct != 1))
//		{
//			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//			gotoxy(4,1);
//			for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + 18); i++)
//				disp_char(membuf[i]);
//
//			hex_asc(temprec.temprec.quantity, asc, 6);
//			rem_zeros(asc, 2);
//			gotoxy(5,1);
//			lcdprint("QTY: ");
//			if(temprec.temprec.unit_fract)
//			{
//				for(i = 0; i < 6; i++)
//				{
//					if(i == 3)
//						disp_char('.');
//					disp_char(asc[i]);
//				}
//			}
//			else
//			{
//				for(i = 0; i < 3; i++)
//					disp_char(asc[i]);
//			}
//			FlashReadData((((UINT32)UNIT_TYPE_PG * PGBYTS) + ((UINT32)temprec.temprec.unit_type * UNIT_TYPE_LEN)), membuf, UNIT_TYPE_LEN);
//			disp_char(' ');
//			for(i = UNIT_TYPE_SYMBOL; i < (UNIT_TYPE_SYMBOL + 2); i++)
//				disp_char(membuf[i]);
//
//			value_hex_asc_64U = temprec.temprec.final_prod_amount;
//			hex_asc_64U(asc, 12);
//			rem_zeros(asc, 8);
//			gotoxy(6,1);
//			lcdprint("AMT: ");
//			for(i = 0; i < 12; i++)
//			{
//				if(i == 10)
//					disp_char('.');
//				disp_char(asc[i]);
//			}
//		}
//		else if((membuf[PRODUCT_EN_FLAG] != ','))
//		{
//			gotoxy(4,1);
//			lcdprint("   DELETED PRODUCT  ");
//		}
//		else
//		{
//			gotoxy(4,1);
//			lcdprint("NO PRODUCT TO RETURN");
//		}
//
//		gotoxy(7,1);
//		lcdprint("SELECT PRD TO RETURN");
//
//		gotoxy(8,1);
//		lcdprint("PRESS    v/^/CNL/ENT");
//
//		while(!(key = keyPressCheck()));
//
//		switch(key)
//		{
//			case UP_ARROW:
//							if(scrollRecord > 0)
//							{
//								scrollRecord--;
//							}
//							for(i = 0; i < RECDAT_LEN; i++)
//								temprec.arr[i] = recArray[scrollRecord][i];
//							found = 0;
//							if(temprec.temprec.returnedProduct == 1)
//							{
//								for(j = (scrollRecord - 1); j >= 0; j--)
//								{
//									for(i = 0; i < RECDAT_LEN; i++)
//										temprec.arr[i] = recArray[j][i];
//									if(temprec.temprec.returnedProduct != 1)
//									{
//										found = 1;
//										scrollRecord = j;
//										break;
//									}
//								}
//
//								if(!found)
//								{
//									for(j = (scrollRecord + 1); j < recArrayPtr; j++)
//									{
//										for(i = 0; i < RECDAT_LEN; i++)
//											temprec.arr[i] = recArray[j][i];
//										if(temprec.temprec.returnedProduct != 1)
//										{
//											found = 1;
//											scrollRecord = j;
//											break;
//										}
//									}
//								}
//							}
//							break;
//			case DOWN_ARROW:
//							if(scrollRecord < (recArrayPtr - 1))
//							{
//								scrollRecord++;
//							}
//							for(i = 0; i < RECDAT_LEN; i++)
//								temprec.arr[i] = recArray[scrollRecord][i];
//							found = 0;
//							if(temprec.temprec.returnedProduct == 1)
//							{
//								for(j = (scrollRecord + 1); j < recArrayPtr; j++)
//								{
//									for(i = 0; i < RECDAT_LEN; i++)
//										temprec.arr[i] = recArray[j][i];
//									if(temprec.temprec.returnedProduct != 1)
//									{
//										found = 1;
//										scrollRecord = j;
//										break;
//									}
//								}
//
//								if(!found)
//								{
//									for(j = (scrollRecord - 1); j >= 0; j--)
//									{
//										for(i = 0; i < RECDAT_LEN; i++)
//											temprec.arr[i] = recArray[j][i];
//										if(temprec.temprec.returnedProduct != 1)
//										{
//											found = 1;
//											scrollRecord = j;
//											break;
//										}
//									}
//								}
//							}
//							break;
//			case '*':
//							exit = 1;
//							break;
//			case '#':
//							FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//							if(membuf[PRODUCT_EN_FLAG] == ',')
//							{
//								if(mc.mc.reverse_tax_en_dis)
//								{
//									temprec.temprec.rate = temprec.temprec.manualRate;
//								}
//								tempQty = get_QUANTITY(temprec.temprec.quantity);
//								tempvar64U = mult64_32_32(tempQty, temprec.temprec.rate);
//								tempAmount = (tempvar64U);
//								if((!get_dig_esc_flag) && (!get_dig_dot_esc_key))
//								{
//									if((tempQty < temprec.temprec.quantity) && (tempQty > 0))
//									{
//										if(recArrayPtr > MAX_TEMP_REC_RET)
//										{
//											clrscr();
//											double_height = 1;
//											double_width = 0;
//											gotoxy(3,1);
//											lcdprint("  MAX ITEMS REACHED ");
//											beep(ERRRBEEP);
//											Delay_ms(3000);
//											break;
//										}
//										temprec.temprec.quantity -= tempQty;
//										tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
//										temprec.temprec.final_prod_amount  = (tempvar64U);
//										temprec.temprec.returnedProduct = 0;
//										store_entry_return_bill();
//										for(i = 0; i < RECDAT_LEN; i++)
//											recArray[scrollRecord][i] = temprec.arr[i];
//
//										temprec.temprec.quantity = tempQty;
//										temprec.temprec.final_prod_amount = tempAmount;
//										temprec.temprec.returnedProduct = 1;
//										store_entry_return_bill();
//										for(i = 0; i < RECDAT_LEN; i++)
//											recArray[recArrayPtr][i] = temprec.arr[i];
//										recArrayPtr++;
//										g_returnCount++;
//									}
//									else if(tempQty == temprec.temprec.quantity)
//									{
//										if(recArrayPtr > MAX_TEMP_REC_RET)
//										{
//											clrscr();
//											double_height = 1;
//											double_width = 0;
//											gotoxy(3,1);
//											lcdprint("  MAX ITEMS REACHED ");
//											beep(ERRRBEEP);
//											Delay_ms(3000);
//											break;
//										}
//										temprec.temprec.quantity = tempQty;
//										temprec.temprec.final_prod_amount = tempAmount;
//										temprec.temprec.returnedProduct = 1;
//										store_entry_return_bill();
//										for(i = 0; i < RECDAT_LEN; i++)
//											recArray[scrollRecord][i] = temprec.arr[i];
//										g_returnCount++;
//									}
//									else
//									{
//										clrscr();
//										double_height = 1;
//										double_width = 0;
//										gotoxy(3,1);
//										lcdprint("ENTERTED QTY EXCEEDS");
//										gotoxy(5,1);
//										lcdprint("      SALE QTY      ");
//										beep(SHRTBEEP);
//										Delay_ms(2000);
//									}
//								}
//
//								for(i = 0; i < RECDAT_LEN; i++)
//									temprec.arr[i] = recArray[scrollRecord][i];
//								found = 0;
//								if(temprec.temprec.returnedProduct == 1)
//								{
//									for(j = (scrollRecord + 1); j < recArrayPtr; j++)
//									{
//										for(i = 0; i < RECDAT_LEN; i++)
//											temprec.arr[i] = recArray[j][i];
//										if(temprec.temprec.returnedProduct != 1)
//										{
//											found = 1;
//											scrollRecord = j;
//											break;
//										}
//									}
//
//									if(!found)
//									{
//										for(j = (scrollRecord - 1); j >= 0; j--)
//										{
//											for(i = 0; i < RECDAT_LEN; i++)
//												temprec.arr[i] = recArray[j][i];
//											if(temprec.temprec.returnedProduct != 1)
//											{
//												found = 1;
//												scrollRecord = j;
//												break;
//											}
//										}
//									}
//								}
//							}
//							else
//							{
//								clrscr();
//								double_height = 1;
//								double_width = 0;
//								gotoxy(3,1);
//								lcdprint("   PRODUCT DELETED  ");
//								gotoxy(5,1);
//								lcdprint("    CANNOT RETURN   ");
//								beep(SHRTBEEP);
//								Delay_ms(2000);
//							}
//							break;
//			case PRINT_KEY:
//							if(g_returnCount)
//							{
//								g_returnCount = 0;
//								g_actualCount = 0;
//								for(j = 0; j < recArrayPtr; j++)
//								{
//									for(i = 0; i < RECDAT_LEN; i++)
//										temprec.arr[i] = recArray[j][i];
//									temprec.temprec.bill_stat = 'O';
//									temprec.temprec.udf_no = udf_no;
//									for(i = 0; i < RECDAT_LEN; i++)
//										recArray[j][i] = temprec.arr[i];
//									if(temprec.temprec.returnedProduct == 1)
//									{
//										g_returnCount++;
//									}
//									else
//									{
//										g_actualCount++;
//									}
//								}
//								saveOrderReturnBill(0);
//								if(g_actualCount)
//								{
//									store_last_rec_returnBill(0);
//									if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
//									{
//										if(mc.mc.rcpt_no >= MAX_RCPTNO)
//											mc.mc.rcpt_no = 1;
//										else
//											mc.mc.rcpt_no++;
//									}
//									store_mc();
//								}
//
//								returnBill = 1;
//								store_last_rec_returnBill(1);
//								returnBill = 0;
//								if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
//								{
//									if(mc.mc.rcpt_no >= MAX_RCPTNO)
//										mc.mc.rcpt_no = 1;
//									else
//										mc.mc.rcpt_no++;
//								}
//
//								if(mc.mc.stock_manage_en_dis)
//								{
//									loadOrderDetailsIntoRAM();
//									manageStockFronRAMAndStore(1);
//								}
//
//								voidBillInBulk(recptNo, returnPageFlag);
//								voidBWRReportBills(recptNo, recptNo, returnPageFlag);
//
//								if(g_actualCount)
//								{
//									returnBill = 0;
//									loadReturnBillDetailsIntoRAM(0);
//									if(mc.mc.bill_format_selection == TAX_SORTING)
//									{
//										printNonUDFReturnBill(0);
//									}
//									else
//									{
//										bill_print_return(0);
//									}
//									clrscr();
//									double_height = 1;
//									double_width = 0;
//									gotoxy(3,1);
//									lcdprint("  PRESS ANY KEY TO  ");
//									gotoxy(5,1);
//									lcdprint("  PRINT RETURN BILL ");
//									while(!(key = keyPressCheck()));
//								}
//
//								returnBill = 1;
//								loadReturnBillDetailsIntoRAM(1);
//								if(mc.mc.bill_format_selection == TAX_SORTING)
//								{
//									printNonUDFReturnBill(1);
//								}
//								else
//								{
//									bill_print_return(1);
//								}
//								if(mc.mc.drawer_en_dis) //VIKAS
//								{
//									DRW_ENABLE = 1;
//									Delay_ms(200);
//									DRW_ENABLE = 0;
//								}
//
//								returnBill = 0;
//
//								mc.mc.rec_start = 0;
//								mc.mc.selected = 0;
//								selectedAmount = 0;
//								selectedQty = 0;
//								selectedProd = 0;
//								recArrayPtr = 0;
//								returnBill = 0;
//								store_mc();
//								returnBill = 0;
//								exit = 1;
//								udf_char = 0;
//								if(mc.mc.gprs_en_dis == RECORD_LOT)
//								{
//									if(mc.mc.gprs_record_sent >= mc.mc.gprs_recordsLot)
//									{
//										sendDataGPRS(0);
//									}
//								}
//								if(mc.mc.gsm_en_dis == RECORD_LOT)
//								{
//									if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
//									{
//										sendDayWiseSummary_SMSToAllMobileNumbers();
//									}
//								}
//								udf_no = 0;
//							}
//							break;
//			default:
//							break;
//		}
//	}
//}
////-------------------------------------------------------------------------
//void saveOrderReturnBill(unsigned char billType)
//{
//	UINT16 count = 0, i = 0, recCountLoop = 0;
//	UINT32 endAdd = 0, dataCount = 0, recCount = 0, currentReceiptNO = 0;
//
//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint("   SAVING RECORDS   ");
//
//	if(mc.mc.udf_en_dis && (!calculatorMode))
//	{
//		get_udf(udf_no);
//		udfno.udfno.udfActive = 1;
//		if(!udfno.udfno.udfStart)
//		{
//			if(mc.mc.currentPG)
//				udfno.udfno.udfStart = mc.mc.recs;
//			else
//				udfno.udfno.udfStart = mc.mc.rec1;
//		}
//		udfno.udfno.udfRcptNo = mc.mc.rcpt_no;
//		store_udf(udf_no);
//
//		if(mc.mc.rcpt_no >= MAX_RCPTNO)
//			mc.mc.rcpt_no = 1;
//		else
//			mc.mc.rcpt_no++;
//		store_mc();
//		rec.rec.rcpt_no = udfno.udfno.udfRcptNo;
//	}
//	else
//	{
//		rec.rec.rcpt_no = mc.mc.rcpt_no;
//	}
//
//	if((recArrayPtr > 0))
//	{
//		if(mc.mc.currentPG)
//			startProd = mc.mc.recs;
//		else
//			startProd = mc.mc.rec1;
//
//		if(!mc.mc.rec_start)
//		{
//			mc.mc.rec_start = startProd;
//			store_mc();
//		}
//		endAdd = startProd;
//
//		selectedProd = 0;
//
//		for(count = 0, recCount = 0; count < recArrayPtr; count++)
//		{
//			for(i = 0; i < RECDAT_LEN; i++)
//				rec.arr[i] = recArray[count][i];
//			if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//			{
//				recCount++;
//			}
//		}
//
//		currentReceiptNO = mc.mc.rcpt_no;
//
//		memset(g_8K_membuf, 0, 4096);
//		for(count = 0, dataCount = 0; count < recArrayPtr; count++)
//		{
//			for(i = 0; i < RECDAT_LEN; i++)
//				rec.arr[i] = recArray[count][i];
//			if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//			{
//				if(recCount == recArrayPtr)
//					rec.rec.rcpt_no = currentReceiptNO;
//				else
//					rec.rec.rcpt_no = currentReceiptNO + 1;
//
//				rec.rec.returnedProduct = 1;
//				rec.rec.calc_stat = 'R';
//
//				for(i = 0; i < RECDAT_LEN; i++)
//					g_8K_membuf[dataCount++] = rec.arr[i];
//
//				selectedProd++;
//				recCountLoop++;
//
//			}
//			else if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//			{
//				rec.rec.rcpt_no = currentReceiptNO;
//
//				for(i = 0; i < RECDAT_LEN; i++)
//					g_8K_membuf[dataCount++] = rec.arr[i];
//
//				selectedProd++;
//				recCountLoop++;
//			}
//
//			if(dataCount > TEMP_REC_PUSH_ARRAY)
//			{
//				if(mc.mc.currentPG)
//					FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
//				else
//					FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
//				endAdd += recCountLoop;
//				recCountLoop = 0;
//				dataCount = 0;
//			}
//			else if((dataCount <= TEMP_REC_PUSH_ARRAY) && (count == (recArrayPtr - 1)))
//			{
//				if(mc.mc.currentPG)
//					FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
//				else
//					FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
//				endAdd += recCountLoop;
//				recCountLoop = 0;
//				dataCount = 0;
//			}
//		}
//
//		if(mc.mc.currentPG)
//			mc.mc.recs = endAdd;
//		else
//			mc.mc.rec1 = endAdd;
//		store_mc();
//		memset(g_8K_membuf, 0, 4096);
//	}
//}
//-------------------------------------------------------------------------
unsigned long enter_purchase_amount(void)
{
	UINT32 input_rate = 0;

	clrscr();
	double_height = 0;
	gotoxy(8,1);
	lcdprint("PRESS       CNL/ENT");

	double_height = 1;
	gotoxy(1,1);
	lcdprint("ENTER PURCHASE AMT ");

	recordedDotPosition = 0;
	get_dig_dot_esc_key = 0;
	input_rate = get_dig_dot(4,6,7,2,0,1);

	if(input_rate)
		return input_rate;
	else
		return 0;
}
//-------------------------------------------------------------------------
void viewBillAmount(unsigned int selectedUdfNo)
{
	UCHAR i = 0, j = 0;
	if(recArrayPtr > 0)
	{
		get_udf(selectedUdfNo);
		if(udfno.udfno.udfActive)
		{
			loadBillAmountRAM();
		}
		for(i = 0, selectedAmount = 0; i < recArrayPtr; i++)
		{
			for(j = 0; j < RECDAT_LEN; j++)
					temprec.arr[j] = recArray[i][j];
			if(temprec.temprec.bill_stat == 'O')
			{
				selectedAmount += temprec.temprec.final_prod_amount;
				if(!mc.mc.reverse_tax_en_dis)
				{
					selectedAmount += temprec.temprec.sgst_amt;
					selectedAmount += temprec.temprec.cgst_amt;
				}
				if(temprec.temprec.discount_amt)		//Mrugnil 15/04/25
					selectedAmount -= temprec.temprec.discount_amt;		//Mrugnil 15/04/25
			}
		}
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("    BILL DETAILS    ");
		gotoxy(3,1);
		FlashReadData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME), asc, UDF_NAME_LEN);
		for(i = 0; i < 10; i++)
		{
			disp_char(asc[i]);
		}
		disp_char(':');
		disp_char(' ');
		hex_asc(selectedUdfNo, asc, 3);
		rem_zeros(asc,2);
		for(i = 0; i < 3; i++)
			disp_char(asc[i]);
		gotoxy(5,1);
		lcdprint("AMT : ");
		value_hex_asc_64U = selectedAmount;
		hex_asc_64U(asc, 11);
		rem_zeros(asc, 7);
		if(selectedAmount > 0)
		{
			for(i = 0; i < 11; i++)
			{
				if(i == 9)
					disp_char('.');
				disp_char(asc[i]);
			}
		}
		double_height = 0;
		gotoxy(8,1);
		lcdprint("  PRESS ANY TO EXIT ");
		while(!keyPressCheck());
	}
	else
	{
		get_udf(selectedUdfNo);
		if(udfno.udfno.udfActive)
		{
			loadOrderDetailsIntoRAM();
			if(recArrayPtr > 0)
			{
				recArrayPtr = 0;
				clrscr();
				double_height = 1;
				double_width = 0;
				gotoxy(1,1);
				lcdprint("    BILL DETAILS    ");
				gotoxy(3,1);
				FlashReadData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME), asc, UDF_NAME_LEN);
				for(i = 0; i < 10; i++)
				{
					disp_char(asc[i]);
				}
				disp_char(':');
				disp_char(' ');
				hex_asc(selectedUdfNo, asc, 3);
				rem_zeros(asc,2);
				for(i = 0; i < 3; i++)
					disp_char(asc[i]);
				gotoxy(5,1);
				lcdprint("AMT : ");
				value_hex_asc_64U = selectedAmount;
				hex_asc_64U(asc, 11);
				rem_zeros(asc, 7);
				if(selectedAmount > 0)
				{
					for(i = 0; i < 11; i++)
					{
						if(i == 9)
							disp_char('.');
						disp_char(asc[i]);
					}
				}
				double_height = 0;
				gotoxy(8,1);
				lcdprint("  PRESS ANY TO EXIT ");
				while(!keyPressCheck());
			}
		}
	}
}
//-------------------------------------------------------------------------
void loadReturnBillDetailsIntoRAM(unsigned char billType)
{
	UINT32 loopRec = 0, startAdd = 0, endAdd = 0;
	UCHAR i = 0;

	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);
		if(udfno.udfno.udfActive)
			startAdd = udfno.udfno.udfStart;
	}
	else
	{
		startAdd = mc.mc.rec_start;
	}
	recArrayPtr = 0;
	sgstArrayPtr = 0;
	cgstArrayPtr = 0;
	g_prodPosArrayPtr = 0;
	g_prodArrayPtr = 0;
	if(mc.mc.currentPG)
		endAdd = mc.mc.recs;
	else
		endAdd = mc.mc.rec1;
	selectedAmount = 0;
	for(loopRec = startAdd; loopRec < endAdd; loopRec++)
	{
		get_rec(loopRec);

		if(mc.mc.udf_en_dis)
		{
			if(billType)
			{
				if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no) && (rec.rec.returnedProduct == 1))
				{
					for(i = 0; i < RECDAT_LEN; i++)
						recArray[recArrayPtr][i] = rec.arr[i];
					recArrayPtr++;
					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
					selectedAmount += rec.rec.final_prod_amount;
					if(!mc.mc.reverse_tax_en_dis)
					{
						selectedAmount += rec.rec.sgst_amt;
						selectedAmount += rec.rec.cgst_amt;
					}
					if(rec.rec.discount_amt)		//Mrugnil 15/04/25
						selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
				}
			}
			else
			{
				if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no) && (rec.rec.returnedProduct != 1))
				{
					for(i = 0; i < RECDAT_LEN; i++)
						recArray[recArrayPtr][i] = rec.arr[i];
					recArrayPtr++;
					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
					selectedAmount += rec.rec.final_prod_amount;
					if(!mc.mc.reverse_tax_en_dis)
					{
						selectedAmount += rec.rec.sgst_amt;
						selectedAmount += rec.rec.cgst_amt;
					}
					if(rec.rec.discount_amt)		//Mrugnil 15/04/25
						selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
				}
			}
		}
		else
		{
			if(billType)
			{
				if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
				{
					for(i = 0; i < RECDAT_LEN; i++)
						recArray[recArrayPtr][i] = rec.arr[i];
					recArrayPtr++;
					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
					selectedAmount += rec.rec.final_prod_amount;
					if(!mc.mc.reverse_tax_en_dis)
					{
						selectedAmount += rec.rec.sgst_amt;
						selectedAmount += rec.rec.cgst_amt;
					}
					if(rec.rec.discount_amt)		//Mrugnil 15/04/25
						selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
				}
			}
			else
			{
				if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
				{
					for(i = 0; i < RECDAT_LEN; i++)
						recArray[recArrayPtr][i] = rec.arr[i];
					recArrayPtr++;
					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
					selectedAmount += rec.rec.final_prod_amount;
					if(!mc.mc.reverse_tax_en_dis)
					{
						selectedAmount += rec.rec.sgst_amt;
						selectedAmount += rec.rec.cgst_amt;
					}
					if(rec.rec.discount_amt)		//Mrugnil 15/04/25
						selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
				}
			}
		}
	}

	sortVATAndServiceArray();
}
//-------------------------------------------------------------------------
void loadBillAmountRAM(void)
{
	UINT32 loopRec = 0, startAdd = 0, endAdd = 0;

	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);
		if(udfno.udfno.udfActive)
			startAdd = udfno.udfno.udfStart;
	}
	else
	{
		startAdd = mc.mc.rec_start;
	}
	if(mc.mc.currentPG)
		endAdd = mc.mc.recs;
	else
		endAdd = mc.mc.rec1;
	selectedAmount = 0;
	for(loopRec = startAdd; loopRec < endAdd; loopRec++)
	{
		get_rec(loopRec);

		if(mc.mc.udf_en_dis)
		{
			if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no))
			{
				selectedAmount += rec.rec.final_prod_amount;
				if(!mc.mc.reverse_tax_en_dis)
				{
					selectedAmount += rec.rec.sgst_amt;
					selectedAmount += rec.rec.cgst_amt;
				}
				if(rec.rec.discount_amt)		//Mrugnil 15/04/25
					selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
			}
		}
		else
		{
			if((rec.rec.bill_stat == 'O'))
			{
				selectedAmount += rec.rec.final_prod_amount;
				if(!mc.mc.reverse_tax_en_dis)
				{
					selectedAmount += rec.rec.sgst_amt;
					selectedAmount += rec.rec.cgst_amt;
				}
				if(rec.rec.discount_amt)		//Mrugnil 15/04/25
					selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
			}
		}
	}
}
//-------------------------------------------------------------------------
void loadOrderDetailsIntoRAM(void)
{
	UINT32 loopRec = 0, startAdd = 0, endAdd = 0;
	UCHAR i = 0;

	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);
		if(udfno.udfno.udfActive)
			startAdd = udfno.udfno.udfStart;
	}
	else
	{
		startAdd = mc.mc.rec_start;
	}
	recArrayPtr = 0;
	sgstArrayPtr = 0;
	cgstArrayPtr = 0;
	g_prodPosArrayPtr = 0;
	g_prodArrayPtr = 0;
	if(mc.mc.currentPG)
		endAdd = mc.mc.recs;
	else
		endAdd = mc.mc.rec1;
	selectedAmount = 0;
	for(loopRec = startAdd; loopRec < endAdd; loopRec++)
	{
		get_rec(loopRec);

		if(mc.mc.udf_en_dis)
		{
			if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no))
			{
				for(i = 0; i < RECDAT_LEN; i++)
					recArray[recArrayPtr][i] = rec.arr[i];
				recArrayPtr++;
				sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
				cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
				g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
				selectedAmount += rec.rec.final_prod_amount;
				if(!mc.mc.reverse_tax_en_dis)
				{
					selectedAmount += rec.rec.sgst_amt;
					selectedAmount += rec.rec.cgst_amt;
				}
				if(rec.rec.discount_amt)		//Mrugnil 15/04/25
					selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
				prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
				prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
				flagIGST = rec.rec.igstFlag;
			}
		}
		else
		{
			if((rec.rec.bill_stat == 'O'))
			{
				for(i = 0; i < RECDAT_LEN; i++)
					recArray[recArrayPtr][i] = rec.arr[i];
				recArrayPtr++;
				sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
				cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
				g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
				selectedAmount += rec.rec.final_prod_amount;
				if(!mc.mc.reverse_tax_en_dis)
				{
					selectedAmount += rec.rec.sgst_amt;
					selectedAmount += rec.rec.cgst_amt;
				}
				if(rec.rec.discount_amt)		//Mrugnil 15/04/25
					selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25
				prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
				prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
				flagIGST = rec.rec.igstFlag;
			}
		}
	}

	sortVATAndServiceArray();
}
//-------------------------------------------------------------------------
void loadDuplicateBillDetailsIntoRAM(unsigned long start, unsigned long last, unsigned int rcptNO, unsigned char page)
{
	UINT32 loopRec = 0;
	UCHAR i = 0;

	recArrayPtr = 0;
	sgstArrayPtr = 0;
	cgstArrayPtr = 0;
	g_prodPosArrayPtr = 0;
	g_prodArrayPtr = 0;
	for(loopRec = start; loopRec < last; loopRec++)
	{
		get_rec_report(loopRec, page);
		if((rec.rec.bill_stat == 'C') && (rec.rec.rcpt_no == rcptNO))
		{
			for(i = 0; i < RECDAT_LEN; i++)
				recArray[recArrayPtr][i] = rec.arr[i];
			recArrayPtr++;
			sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
			cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
			g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
			prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
			prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
			flagIGST = rec.rec.igstFlag;
		}

	}

	sortVATAndServiceArray();
}
//-------------------------------------------------------------------------
void deductStockReturnBill(unsigned char flag)
{
	UCHAR i = 0, tempBuf[PRODUCT_LEN], productStockSign = 0, tempBuf1[PRODUCT_STOCK_LEN], tempCount = 0;
	UINT32 tempQTY = 0, productStock = 0, entertedQty = 0;
	UINT16 count = 0;

	loadOrderDetailsIntoRAM();

	for(count = 0; count < (recArrayPtr); count++)
	{
		for(i = 0; i < RECDAT_LEN; i++)
			temprec.arr[i] = recArray[count][i];

		if((temprec.temprec.last != 1) && (temprec.temprec.returnedProduct != 1))
		{
			productStockSign = temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign;
			productStock = temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock;

			if(mc.mc.qty_enter_method == QTY_3_3)
				entertedQty = (temprec.temprec.quantity / 10);

			if(flag)
			{
				if(productStockSign)
				{
					tempQTY = (productStock + entertedQty);
				}
				else
				{
					if(entertedQty > productStock)
					{
						tempQTY = (entertedQty - productStock);
						productStockSign = 1;
					}
					else
					{
						tempQTY = (productStock - entertedQty);
					}
				}
			}
			else
			{
				if(productStockSign)
				{
					if(entertedQty > productStock)
					{
						tempQTY = (entertedQty - productStock);
						productStockSign = 0;
					}
					else
						tempQTY = (productStock - entertedQty);
				}
				else
				{
					tempQTY = (productStock + entertedQty);
				}
			}

			temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign = productStockSign;
			temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock = tempQTY;
		}
	}
}
//-------------------------------------------------------------------------
void manageTAXFromRAMAndStore(unsigned char taxType, unsigned char addFlag)
{
	UINT16 loop = 0, loopCount = 0, i = 0;
	for(loop = 0; (loop < TEMP_TAX_SUMM_ARRAY) && (loop < mc.mc.sgst_summ_count); loop++)
	{
		for(loopCount = 0; loopCount < recArrayPtr; loopCount++)
		{
			for(i = 0; i < RECDAT_LEN; i++)
				temprec.arr[i] = recArray[loopCount][i];

			if(taxType == SGST)
			{
				if((temprec.temprec.prd_sgst == SGST_tax_summary[loop].tax_summary.rate) && (temprec.temprec.returnedProduct != 1))
				{
					if(temprec.temprec.prd_sgst)
					{
						if(returnBill)
						{
							if(SGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.sgst_amt)
								SGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.sgst_amt;
							else
								SGST_tax_summary[loop].tax_summary.total_amount = 0;
						}
						else
						{
							if(addFlag)
							{
								SGST_tax_summary[loop].tax_summary.total_amount += temprec.temprec.sgst_amt;
							}
							else
							{
								if(SGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.sgst_amt)
									SGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.sgst_amt;
								else
									SGST_tax_summary[loop].tax_summary.total_amount = 0;
							}
						}
					}
					else
					{
						if(returnBill)
						{
							if(SGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.final_prod_amount)
								SGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.final_prod_amount;
							else
								SGST_tax_summary[loop].tax_summary.total_amount = 0;
						}
						else
						{
							if(addFlag)
							{
								SGST_tax_summary[loop].tax_summary.total_amount += temprec.temprec.final_prod_amount;
							}
							else
							{
								if(SGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.final_prod_amount)
									SGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.final_prod_amount;
								else
									SGST_tax_summary[loop].tax_summary.total_amount = 0;
							}
						}
					}
				}
			}
			else if(taxType == CGST)
			{
				if((temprec.temprec.prd_cgst == CGST_tax_summary[loop].tax_summary.rate) && (temprec.temprec.returnedProduct != 1))
				{
					if(temprec.temprec.prd_cgst)
					{
						if(returnBill)
						{
							if(CGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.cgst_amt)
								CGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.cgst_amt;
							else
								CGST_tax_summary[loop].tax_summary.total_amount = 0;
						}
						else
						{
							if(addFlag)
							{
								CGST_tax_summary[loop].tax_summary.total_amount += temprec.temprec.cgst_amt;
							}
							else
							{
								if(CGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.cgst_amt)
									CGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.cgst_amt;
								else
									CGST_tax_summary[loop].tax_summary.total_amount = 0;
							}
						}
					}
					else
					{
						if(returnBill)
						{
							if(CGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.final_prod_amount)
								CGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.final_prod_amount;
							else
								CGST_tax_summary[loop].tax_summary.total_amount = 0;
						}
						else
						{
							if(addFlag)
							{
								CGST_tax_summary[loop].tax_summary.total_amount += temprec.temprec.final_prod_amount;
							}
							else
							{
								if(CGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.final_prod_amount)
									CGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.final_prod_amount;
								else
									CGST_tax_summary[loop].tax_summary.total_amount = 0;
							}
						}
					}
				}
			}
			else if(taxType == IGST)
			{
				if((temprec.temprec.prd_cgst == IGST_tax_summary[loop].tax_summary.rate) && (temprec.temprec.returnedProduct != 1))
				{
					if(temprec.temprec.prd_cgst)
					{
						if(returnBill)
						{
							if(IGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.cgst_amt)
								IGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.cgst_amt;
							else
								IGST_tax_summary[loop].tax_summary.total_amount = 0;
						}
						else
						{
							if(addFlag)
							{
								IGST_tax_summary[loop].tax_summary.total_amount += temprec.temprec.cgst_amt;
							}
							else
							{
								if(IGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.cgst_amt)
									IGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.cgst_amt;
								else
									IGST_tax_summary[loop].tax_summary.total_amount = 0;
							}
						}
					}
					else
					{
						if(returnBill)
						{
							if(IGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.final_prod_amount)
								IGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.final_prod_amount;
							else
								IGST_tax_summary[loop].tax_summary.total_amount = 0;
						}
						else
						{
							if(addFlag)
							{
								IGST_tax_summary[loop].tax_summary.total_amount += temprec.temprec.final_prod_amount;
							}
							else
							{
								if(IGST_tax_summary[loop].tax_summary.total_amount > temprec.temprec.final_prod_amount)
									IGST_tax_summary[loop].tax_summary.total_amount -= temprec.temprec.final_prod_amount;
								else
									IGST_tax_summary[loop].tax_summary.total_amount = 0;
							}
						}
					}
				}
			}
		}
	}
}
//-------------------------------------------------------------------------
void manageStockFronRAMAndStore(unsigned char subFlag)
{
	UINT16 STOCK_START_INDEX = 0, loopProdSumm = 0, loop = 0, loopCount = 0;
	UCHAR found = 0, i = 0, sign = 0;
	UINT32 enteredQTY = 0, finalQty = 0, productStock = 0;

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

	for(loop = 0; loop < recArrayPtr; loop++)
	{
		for(i = 0; i < RECDAT_LEN; i++)
			temprec.arr[i] = recArray[loop][i];
		if((temprec.temprec.last != 1) && (temprec.temprec.returnedProduct != 1))
		{
			if(mc.mc.qty_enter_method == QTY_3_3)
				enteredQTY = (temprec.temprec.quantity / 10);
			else
				enteredQTY = (temprec.temprec.quantity);

			productStock = temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock;
			sign = temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign;
			finalQty = 0;
			if(subFlag)
			{
				if(sign)
				{
					finalQty = (productStock + enteredQTY);
				}
				else
				{
					if(enteredQTY > productStock)
					{
						finalQty = (enteredQTY - productStock);
						sign = 1;
					}
					else
					{
						finalQty = (productStock - enteredQTY);
					}
				}
			}
			else
			{
				if(sign)
				{
					if(enteredQTY > productStock)
					{
						finalQty = (enteredQTY - productStock);
						sign = 0;
					}
					else
						finalQty = (productStock - enteredQTY);
				}
				else
				{
					finalQty = (productStock + enteredQTY);
				}
			}

			temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock = finalQty;
			temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign = sign;
		}
	}

	loop = 0;
}
//-------------------------------------------------------------------------
void manageProductSummaryFromRAMAndStore(unsigned char addFlag)
{
	UINT16 SUMMARY_START_ADD = 0, loopProdSumm = 0, loop = 0, loopCount = 0;
	UCHAR found = 0, i = 0;

	for(loop = 0; loop < recArrayPtr; loop++)
	{
		for(i = 0; i < RECDAT_LEN; i++)
			temprec.arr[i] = recArray[loop][i];

		FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
		for(i = PRODUCT_SER_NO; i <(PRODUCT_SER_NO + PRODUCT_SER_NO_LEN); i++)
			asc[i - PRODUCT_SER_NO] = membuf[i];

		temp_product_summary[temprec.temprec.product_code].temp_product_summary.prodCode = asc_hex(asc, PRODUCT_SER_NO_LEN);
		if(addFlag)
		{
			temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_amount += temprec.temprec.final_prod_amount;
			if(mc.mc.qty_enter_method == QTY_3_3)
				temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty += temprec.temprec.quantity;
			else
				temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty += (temprec.temprec.quantity*10);
			temp_product_summary[temprec.temprec.product_code].temp_product_summary.stockBalance = temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock;
			temp_product_summary[temprec.temprec.product_code].temp_product_summary.stock_sign = temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign;
		}
		else
		{
			if(temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_amount >= temprec.temprec.final_prod_amount)
				temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_amount -= temprec.temprec.final_prod_amount;
			else
				temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_amount = 0;

			if(mc.mc.qty_enter_method == QTY_3_3)
			{
				if(temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty >= temprec.temprec.quantity)
					temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty -= temprec.temprec.quantity;
				else
					temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty = 0;
			}
			else
			{
				if(temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty >= (temprec.temprec.quantity*10))
					temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty -= (temprec.temprec.quantity*10);
				else
					temp_product_summary[temprec.temprec.product_code].temp_product_summary.total_qty = 0;
			}

			temp_product_summary[temprec.temprec.product_code].temp_product_summary.stockBalance = temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock;
			temp_product_summary[temprec.temprec.product_code].temp_product_summary.stock_sign = temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign;
		}
	}
}
//-------------------------------------------------------------------------
void managePreTaxDiscountFromRAMAndStore(unsigned long startA, unsigned long preTaxDiscRate)
{
	UINT32 RECORD_START_ADD = 0, loopProdSumm = 0, loop = 0, loopCount = 0, totalRecs = 0, recs = 0, reccount = 0;
	UCHAR found = 0, i = 0;
	UINT64 tempRate = 0, temp_amount = 0, vat_temp = 0, serv = 0;

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	totalRecs = (recs / TEMP_RECS_DISC_ARRAY);
	if((recs % TEMP_RECS_DISC_ARRAY) > 0)
		totalRecs += 1;

	RECORD_START_ADD = startA;
	for(loopProdSumm = 0; loopProdSumm < totalRecs; loopProdSumm++)
	{
		reccount = loadRecordsinTempArray(RECORD_START_ADD);

		if(reccount > recs)
			break;

		found = 0;
		for(loop = RECORD_START_ADD; (loop < recs) && (loop < ((RECORD_START_ADD + TEMP_RECS_DISC_ARRAY))); loop++)
		{
			for(i = 0; i < RECDAT_LEN; i++)
				rec.arr[i] = save_rec[loop - RECORD_START_ADD].arr[i];

			if(mc.mc.udf_en_dis)
			{
				if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O'))
				{
					found = 1;
					rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);
					rec.rec.disc_rate = preTaxDiscRate;//vikas

					if(mc.mc.reverse_tax_en_dis)
					{
						if(rec.rec.prd_sgst)
						{
							rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
						}
						if(rec.rec.prd_cgst)
						{
							rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
						}
					}
					else
					{
						if(rec.rec.prd_sgst)
						{
							rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
						}
						else
							rec.rec.sgst_amt = 0;
						if(rec.rec.prd_cgst)
						{
							rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
						}
						else
							rec.rec.cgst_amt = 0;
					}
				}
			}
			else
			{
				if((rec.rec.bill_stat == 'O'))
				{
					found = 1;
					rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount , preTaxDiscRate, DISCOUNT);

					if(mc.mc.reverse_tax_en_dis)
					{
						if(rec.rec.prd_sgst)
						{
							rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
						}
						if(rec.rec.prd_cgst)
						{
							rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
						}
					}
					else
					{
						if(rec.rec.prd_sgst)
						{
							rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
						}
						else
							rec.rec.sgst_amt = 0;
						if(rec.rec.prd_cgst)
						{
							rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
						}
						else
							rec.rec.cgst_amt = 0;
					}
				}
			}

			for(i = 0; i < RECDAT_LEN; i++)
				save_rec[loop - RECORD_START_ADD].arr[i] = rec.arr[i];
		}
		if(found)
		{
			for(loop = 0, loopCount = 0; (loop < TEMP_RECS_DISC_ARRAY); loop++)
			{
				for(i = 0; i < RECDAT_LEN; i++)
					g_8K_membuf[loopCount++] = save_rec[loop].arr[i];
			}

			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
		}

	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

		RECORD_START_ADD += TEMP_RECS_DISC_ARRAY;
	}
}
//-------------------------------------------------------------------------
void closeOpenBill(unsigned long startA, unsigned long endA, unsigned char calculatorType)
{
	UINT32 RECORD_START_ADD = 0, loopProdSumm = 0, loop = 0, loopCount = 0, totalRecs = 0, recs = 0, reccount = 0;
	UCHAR found = 0, i = 0, done = 0;

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	totalRecs = (recs / TEMP_RECS_DISC_ARRAY);
	if((recs % TEMP_RECS_DISC_ARRAY) > 0)
		totalRecs += 1;

	RECORD_START_ADD = startA;
	for(loopProdSumm = 0; loopProdSumm < totalRecs; loopProdSumm++)
	{
		reccount = loadRecordsinTempArray(RECORD_START_ADD);

		if(reccount > recs)
			break;

		found = 0;
		for(loop = RECORD_START_ADD; (loop < recs) && (loop < ((RECORD_START_ADD + TEMP_RECS_DISC_ARRAY))); loop++)
		{
			for(i = 0; i < RECDAT_LEN; i++)
				rec.arr[i] = save_rec[loop - RECORD_START_ADD].arr[i];

			if(calculatorType)
			{
				if((rec.rec.calc_stat == 'C') && (rec.rec.bill_stat == 'O'))
				{
					found = 1;
					rec.rec.bill_stat = 'C';
				}
			}
			else
			{
				if(mc.mc.udf_en_dis)
				{
					if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O'))
					{
						found = 1;
						rec.rec.bill_stat = 'C';
						if((mc.mc.billKotseries == DIFFERENT_SERIES) && (mc.mc.kot_en_dis))
						{
							rec.rec.rcpt_no = mc.mc.actualRcptNo;
							done = 1;
						}
					}
				}
				else
				{
					if((rec.rec.bill_stat == 'O'))
					{
						found = 1;
						rec.rec.bill_stat = 'C';
					}
				}
			}

			for(i = 0; i < RECDAT_LEN; i++)
				save_rec[loop - RECORD_START_ADD].arr[i] = rec.arr[i];
		}
		if(found)
		{
			for(loop = 0, loopCount = 0; (loop < TEMP_RECS_DISC_ARRAY); loop++)
			{
				for(i = 0; i < RECDAT_LEN; i++)
					g_8K_membuf[loopCount++] = save_rec[loop].arr[i];
			}

			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
		}

		memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

		RECORD_START_ADD += TEMP_RECS_DISC_ARRAY;
	}
	if(done)
	{
		if((mc.mc.udf_en_dis) && (mc.mc.kot_en_dis) && (mc.mc.billKotseries == DIFFERENT_SERIES))
		{
			if(mc.mc.actualRcptNo >= MAX_RCPTNO)
				mc.mc.actualRcptNo = 1;
			else
				mc.mc.actualRcptNo++;
		}
	}
}
//-------------------------------------------------------------------------
void clearProductSummaryForSelectedDay(unsigned char pageSelect)
{
	UINT16 SUMMARY_START_ADD = 0, loopProdSumm = 0, loop = 0, loopCount = 0, arrayPtr = 0;
	UCHAR i = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(1,1);
	lcdprint("  CLEARING SUMMARY  ");
	gotoxy(4,1);
	lcdprint(" COUNT : ");

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	SUMMARY_START_ADD = 1;
	for(loopProdSumm = 1, arrayPtr = 1; loopProdSumm <= mc.mc.prod_totalcount; loopProdSumm++)
	{
		FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) loopProdSumm * PRODUCT_LEN)), membuf, PRODUCT_LEN);

		for(i = PRODUCT_SER_NO; i <(PRODUCT_SER_NO + PRODUCT_SER_NO_LEN); i++)
			asc[i - PRODUCT_SER_NO] = membuf[i];

		get_product_stock(loopProdSumm);

		temp_product_summary[arrayPtr].temp_product_summary.prodCode = asc_hex(asc, PRODUCT_SER_NO_LEN);
		temp_product_summary[arrayPtr].temp_product_summary.total_amount = 0;
		temp_product_summary[arrayPtr].temp_product_summary.total_qty = 0;
		temp_product_summary[arrayPtr].temp_product_summary.stockBalance = product_stock.product_stock.totalStock;
		temp_product_summary[arrayPtr].temp_product_summary.stock_sign = product_stock.product_stock.sign;
		arrayPtr++;

		gotoxy(4,11);
		hex_asc(loopProdSumm, asc, 4);
		rem_zeros(asc, 3);
		disp_char(asc[0]);
		disp_char(asc[1]);
		disp_char(asc[2]);
		disp_char(asc[3]);

		if((arrayPtr > TEMP_PRODUCT_SUMM_ARRAY) || (loopProdSumm == mc.mc.prod_totalcount))
		{
			for(loop = 1, loopCount = 0; loop < arrayPtr; loop++)
			{
				for(i = 0; i < PRODUCT_SUMMARY_LEN; i++)
					g_8K_membuf[loopCount++] = temp_product_summary[loop].arr[i];
			}

			if(pageSelect)
			{
				FlashModifyData((((unsigned long)(PRODUCT_SUMMARY_PG * PGBYTS)) +
						((unsigned long)(SUMMARY_START_ADD * PRODUCT_SUMMARY_LEN))), g_8K_membuf, loopCount);
				store_iwr(SUMMARY_START_ADD, g_8K_membuf, loopCount);
			}
			else
			{
				FlashModifyData((((unsigned long)(PRODUCT_SUMMARY1_PG * PGBYTS)) +
						((unsigned long)(SUMMARY_START_ADD * PRODUCT_SUMMARY_LEN))), g_8K_membuf, loopCount);
				store_iwr(SUMMARY_START_ADD, g_8K_membuf, loopCount);
			}

		    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

			SUMMARY_START_ADD += TEMP_PRODUCT_SUMM_ARRAY;

			arrayPtr = 1;
		}
	}
}
//-------------------------------------------------------------------------
void upadteStock(void)
{
	UCHAR exit = 0, key = 0;
	updateFlashContents();
	while(!exit)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("  STOCK MANAGEMENT  ");
		gotoxy(3,1);
		lcdprint("1. ADD STOCK        ");
		gotoxy(5,1);
		lcdprint("2. REPLACE STOCK    ");
		double_height = 0;
		gotoxy(8,1);
		lcdprint("PRESS        1/2/CNL");

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case '1':
					addStock();
					exit = 1;
					break;
			case '2':
					replaceStock();
					exit = 1;
					break;
			case '*':
					exit = 1;
					break;
			default:
					break;
		}
	}
}
//-------------------------------------------------------------------------
void replaceStock(void)
{
	UCHAR exit = 0, screen = 0, i = 0, j = 0, temp_buff[PRODUCT_STOCK_LEN], once = 0, go_back = 0;
	UINT16 prodpos = 0, count = 1;
	UINT32 qtyNew = 0, temp_rate = 0, total_stock = 0;
	UINT16 prodPosArrayPtr = 0;
	UINT64 temp_final_amt = 0, total_amt = 0;

	prodPosArrayPtr = 0;
	count = 1;

	while(!exit)
	{
		switch(screen)
		{
			case 0:
					product_code = 0;
					countSI = 0;
					if(prodPosArrayPtr >= MAX_EDIT_PRODUCTS)
					{
						screen = 3;
						go_back = 1;
						break;
					}
					if(productSearchStockView(count, 1) == 1)
					{
						unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));
						FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
						unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);

						prodpos = product_code;
						FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodpos * PRODUCT_LEN), membuf, PRODUCT_LEN);
						screen = 1;
					}
					else if(searchProductByName_test(0) == 1)		//Mrugnil 27/06/24
					{
						prodpos = product_code;
						FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodpos * PRODUCT_LEN), membuf, PRODUCT_LEN);
						unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));
						FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
						unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);
						FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodpos * PRODUCT_LEN), membuf, PRODUCT_LEN);
						screen = 1;
					}
					else
						screen = 3;
					break;
			case 1:
					clrscr();
					double_height = 1;
					double_width = 0;
					gotoxy(1,1);
					lcdprint("    REPLACE STOCK   ");
					gotoxy(3,1);
					for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + 18); i++)
						disp_char(membuf[i]);

					get_product_summary(prodpos, mc.mc.currentPG);

					hex_asc(product_summary.product_summary.stockBalance, asc, 7);

					double_height = 0;
					gotoxy(8,1);
					lcdprint("PRESS        ENT/CNL");
					double_height = 1;
					recordedDotPosition = 0;
					qtyNew = 0;
					get_dig_dot_esc_key = 0;
					get_dig_esc_flag = 0;
					if(unit_fract)
						qtyNew = get_dig_dot(6, 5, 5, 2, 0, 1);
					else
					{
						qtyNew = get_dig(6, 5, 5, 0, 1, 0);
						qtyNew = (qtyNew * 100);
					}
					if((!get_dig_dot_esc_key) && (!get_dig_esc_flag))
					{
						screen = 2;
					}
					else
					{
						screen = 0;
					}
					break;
			case 2:
					rec_stock[prodPosArrayPtr].rec_stock.product_code = prodpos;
					rec_stock[prodPosArrayPtr].rec_stock.quantity = qtyNew;
					prodPosArrayPtr++;

					product_stock.product_stock.totalStock = qtyNew;
					product_stock.product_stock.sign = 0;
					store_product_stock(prodpos);

					get_product_summary(prodpos, mc.mc.currentPG);
					product_summary.product_summary.stock_sign = 0;
					product_summary.product_summary.stockBalance = qtyNew;
					store_product_summary(prodpos, mc.mc.currentPG);
					screen = 0;
					count++;
					break;
			case 3:
					if(prodPosArrayPtr > 0)
					{
						load256ProductSummaryinTempArray(1);
						loadProductsStockTempArray(1);
						setLanguage(1);
						once = 0;
						for(prodpos = 0; prodpos < prodPosArrayPtr; prodpos++)
						{
							if(!once)
							{
								disp_printmsg();
								print_head(1,0);
								unicode_prt_buf("              STOCK REPLACE REPORT              ");
								unicode_print(FH);
								print_date_time();
								prt_bar();
								unicode_prt_buf("CODE ITEM NAME                     STOCK ADDED  ");
								unicode_print(DH);
								prt_bar();
								once = 1;
							}
							FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)rec_stock[prodpos].rec_stock.product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);
							for(i = PRODUCT_SER_NO; i < (PRODUCT_SER_NO + PRODUCT_SER_NO_LEN); i++)
								unicode_prnbuf(membuf[i]);
							unicode_prnbuf(' ');
							for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + PRODUCT_TEXT_LEN); i++)
								unicode_prnbuf(membuf[i]);

							unicode_prt_buf("      ");

							hex_asc(rec_stock[prodpos].rec_stock.quantity, asc, 8);
							rem_zeros(asc, 4);
							for(i = 0; i < 8; i++)
							{
								if(i == 6)
									unicode_prnbuf('.');
								unicode_prnbuf(asc[i]);
							}
							unicode_print(DH);
							CheckOnOffKey();
						}
						prt_bar();
						paper_feed();
					}
					prodPosArrayPtr = 0;
					if(!go_back)
					{
						exit = 1;
						return;
					}
					screen = 0;
					go_back = 0;
					break;
		}
		CheckOnOffKey();
	}
}
//-------------------------------------------------------------------------
void addStock(void)
{
	UCHAR exit = 0, screen = 0, i = 0, j = 0, once = 0, go_back = 0;
	UINT16 prodpos = 0, count = 1;
	UINT32 qtyNew = 0, temp_rate = 0, total_stock = 0;
	UINT16 prodPosArrayPtr = 0;
	UINT64 temp_final_amt = 0, total_amt = 0;

	prodPosArrayPtr = 0;
	count = 1;

	while(!exit)
	{
		switch(screen)
		{
			case 0:
					product_code = 0;
					searchProductSingle = 0;
					countSI = 0;
					if(prodPosArrayPtr >= MAX_EDIT_PRODUCTS)
					{
						screen = 3;
						go_back = 1;
						break;
					}
					if(productSearchStockView(count, 1) == 1)
					{
						unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));
						FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
						unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);

						prodpos = product_code;
						FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodpos * PRODUCT_LEN), membuf, PRODUCT_LEN);

						screen = 1;
					}
					else if(searchProductByName_test(0) == 1)		//Mrugnil 27/06/24
					{
						prodpos = product_code;
						FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodpos * PRODUCT_LEN), membuf, PRODUCT_LEN);
						unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));
						FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
						unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);
						FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodpos * PRODUCT_LEN), membuf, PRODUCT_LEN);
						screen = 1;
					}
					else
						screen = 3;
					break;
			case 1:
					clrscr();
					double_height = 1;
					double_width = 0;
					gotoxy(1,1);
					lcdprint("      ADD STOCK     ");
					gotoxy(3,1);
					for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + 18); i++)
						disp_char(membuf[i]);

					get_product_summary(prodpos, mc.mc.currentPG);

					hex_asc(product_summary.product_summary.stockBalance, asc, 7);

					double_height = 0;
					gotoxy(8,1);
					lcdprint("PRESS        ENT/CNL");
					double_height = 1;
					recordedDotPosition = 0;
					qtyNew = 0;
					get_dig_dot_esc_key = 0;
					gotoxy(6,4);
					if(product_summary.product_summary.stock_sign == 1)
						disp_char('-');
					else
						disp_char(' ');
					get_dig_dot_esc_key = 0;
					get_dig_esc_flag = 0;
					if(unit_fract)
						qtyNew = get_dig_dot(6, 5, 5, 2, asc_hex(asc, (PRODUCT_STOCK_LEN - 1)), 1);
					else
					{
						qtyNew = get_dig(6, 5, 5, (asc_hex(asc, (PRODUCT_STOCK_LEN - 1)) / 100), 1, 0);
						qtyNew = (qtyNew * 100);
					}
					if((!get_dig_dot_esc_key) && (!get_dig_esc_flag))
					{
						screen = 2;
					}
					else
					{
						screen = 0;
					}
					break;
			case 2:
					if(mc.mc.qty_enter_method == QTY_3_3)
						increaseStock(prodpos, (qtyNew * 10), 1);
					else
						increaseStock(prodpos, (qtyNew), 1);
					rec_stock[prodPosArrayPtr].rec_stock.product_code = prodpos;
					rec_stock[prodPosArrayPtr].rec_stock.quantity = qtyNew;
					prodPosArrayPtr++;
					screen = 0;
					count++;
					break;
			case 3:
					if(prodPosArrayPtr > 0)
					{
						setLanguage(1);
						load256ProductSummaryinTempArray(1);
						loadProductsStockTempArray(1);
						once = 0;
						for(prodpos = 0; prodpos < prodPosArrayPtr; prodpos++)
						{
							if(!once)
							{
								disp_printmsg();
								print_head(1,0);
								unicode_prt_buf("                STOCK ADD REPORT                ");
								unicode_print(FH);
								print_date_time();
								prt_bar();
								unicode_prt_buf("CODE ITEM NAME                     STOCK ADDED  ");
								unicode_print(DH);
								prt_bar();
								once = 1;
							}
							FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)rec_stock[prodpos].rec_stock.product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);
							for(i = PRODUCT_SER_NO; i < (PRODUCT_SER_NO + PRODUCT_SER_NO_LEN); i++)
								unicode_prnbuf(membuf[i]);
							unicode_prnbuf(' ');
							for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + PRODUCT_TEXT_LEN); i++)
								unicode_prnbuf(membuf[i]);

							unicode_prt_buf("      ");

							hex_asc(rec_stock[prodpos].rec_stock.quantity, asc, 8);
							rem_zeros(asc, 4);
							for(i = 0; i < 8; i++)
							{
								if(i == 6)
									unicode_prnbuf('.');
								unicode_prnbuf(asc[i]);
							}
							unicode_print(DH);

							CheckOnOffKey();
						}
						prt_bar();
						paper_feed();
					}
					prodPosArrayPtr = 0;
					if(!go_back)
					{
						exit = 1;
						return;
					}
					screen = 0;
					go_back = 0;
					break;
		}
		CheckOnOffKey();
	}
}
//-------------------------------------------------------------------------
void increaseStock(unsigned int prdCode, unsigned long qty, unsigned char flag)
{
	UCHAR i = 0, productStockSign = 0, tempCount = 0;
	UINT32 tempQTY = 0, productStock = 0, entertedQty = 0;

	get_product_stock(prdCode);

	productStock = product_stock.product_stock.totalStock;
	productStockSign = product_stock.product_stock.sign;

	if(mc.mc.qty_enter_method == QTY_3_3)
		entertedQty = (qty / 10);
	else
		entertedQty = (qty);

	if(flag)
	{
		if(productStockSign)
		{
			if(entertedQty > productStock)
			{
				tempQTY = (entertedQty - productStock);
				productStockSign = 0;
			}
			else
				tempQTY = (productStock - entertedQty);
		}
		else
		{
			tempQTY = (productStock + entertedQty);
		}
	}
	else
	{
		if(productStockSign)
		{
			tempQTY = (productStock + entertedQty);
		}
		else
		{
			if(entertedQty > productStock)
			{
				tempQTY = (entertedQty - productStock);
				productStockSign = 1;
			}
			else
			{
				tempQTY = (productStock - entertedQty);
			}
		}
	}

	product_stock.product_stock.totalStock = tempQTY;
	product_stock.product_stock.sign = productStockSign;
	store_product_stock(prdCode);

	get_product_summary(prdCode, mc.mc.currentPG);
	product_summary.product_summary.stock_sign = productStockSign;
	product_summary.product_summary.stockBalance = tempQTY;
	store_product_summary(prdCode, mc.mc.currentPG);
}
//-------------------------------------------------------------------------
void sortVATAndServiceArray(void)
{
	UCHAR i = 0,j = 0, k = 0;

	if(sgstArrayPtr > 1)
	{
		for (i = 0; i < sgstArrayPtr; i++)
		{
			for (j = i + 1; j < sgstArrayPtr;)
			{
				if (sgstArray[j] == sgstArray[i])
				{
					for (k = j; k < sgstArrayPtr; k++)
					{
						sgstArray[k] = sgstArray[k + 1];
					}
					sgstArrayPtr--;
				}
				else
					j++;
			}
		}
	}
//	vat_count--;

	if(cgstArrayPtr > 1)
	{
		for (i = 0; i < cgstArrayPtr; i++)
		{
			for (j = i + 1; j < cgstArrayPtr;)
			{
				if (cgstArray[j] == cgstArray[i])
				{
					for (k = j; k < cgstArrayPtr; k++)
					{
						cgstArray[k] = cgstArray[k + 1];
					}
					cgstArrayPtr--;
				}
				else
					j++;
			}
		}
	}
//	service_count--;

	if(g_prodPosArrayPtr > 1)
	{
		for (i = 0; i < g_prodPosArrayPtr; i++)
		{
			for (j = i + 1; j < g_prodPosArrayPtr;)
			{
				if (g_ID_buf[j] == g_ID_buf[i])
				{
					for (k = j; k < g_prodPosArrayPtr; k++)
					{
						g_ID_buf[k] = g_ID_buf[k + 1];
					}
					g_prodPosArrayPtr--;
				}
				else
					j++;
			}
		}
	}
//	prodPosArrayPtr--;

	for (i = 0; i < g_prodArrayPtr; i++)
	{
		for (j = i + 1; j < g_prodArrayPtr;)
		{
			if (prd_array[j].prd_array.product_code == prd_array[i].prd_array.product_code)
			{
			    if (prd_array[j].prd_array.rate == prd_array[i].prd_array.rate)
			    {
			        for (k = j; k < g_prodArrayPtr; k++)
    				{
    					prd_array[k].prd_array.product_code = prd_array[k + 1].prd_array.product_code;
    					prd_array[k].prd_array.rate = prd_array[k + 1].prd_array.rate;
    				}
			        g_prodArrayPtr--;
			    }
			    else
                    j++;
			}
			else
				j++;
		}
	}
}
//-------------------------------------------------------------------------
unsigned int hot_key_prod(unsigned char key_select)
{
	UCHAR temp_pos = 0;
	UINT16 temp_code = 0;

	switch(key_select)
	{
	case 'A':
	case 'B':
		if(shiftFlag)
			key_select = 'B';
		break;
	case 'C':
	case 'D':
		if(shiftFlag)
			key_select = 'D';
		break;
	case 'E':
	case 'F':
		if(shiftFlag)
			key_select = 'F';
		break;
	case 'G':
	case 'H':
		if(shiftFlag)
			key_select = 'H';
		break;
	case 'I':
	case 'J':
		if(shiftFlag)
			key_select = 'J';
		break;
	case 'K':
	case 'L':
		if(shiftFlag)
			key_select = 'L';
		break;
	case 'M':
	case 'N':
		if(shiftFlag)
			key_select = 'N';
		break;
	case 'O':
	case 'P':
		if(shiftFlag)
			key_select = 'P';
		break;
	case 'Q':
	case 'R':
		if(shiftFlag)
			key_select = 'R';
		break;
	case 'S':
	case 'T':
		if(shiftFlag)
			key_select = 'T';
		break;
	case 'U':
	case 'V':
		if(shiftFlag)
			key_select = 'V';
		break;
	case 'W':
	case 'X':
		if(shiftFlag)
			key_select = 'X';
		break;
	case 'Y':
	case 'Z':
		if(shiftFlag)
			key_select = 'Z';
		break;
	}

	switch(key_select)
	{
	case 'A':
		temp_pos = 1;
		break;
	case 'B':
		temp_pos = 13 + 1;
		break;
	case 'C':
		temp_pos = 2;
		break;
	case 'D':
		temp_pos = 13 + 2;
		break;
	case 'E':
		temp_pos = 3;
		break;
	case 'F':
		temp_pos = 13 + 3;
		break;
	case 'G':
		temp_pos = 4;
		break;
	case 'H':
		temp_pos = 13 + 4;
		break;
	case 'I':
		temp_pos = 5;
		break;
	case 'J':
		temp_pos = 13 + 5;
		break;
	case 'K':
		temp_pos = 6;
		break;
	case 'L':
		temp_pos = 13 + 6;
		break;
	case 'M':
		temp_pos = 7;
		break;
	case 'N':
		temp_pos = 13 + 7;
		break;
	case 'O':
		temp_pos = 8;
		break;
	case 'P':
		temp_pos = 13 + 8;
		break;
	case 'Q':
		temp_pos = 9;
		break;
	case 'R':
		temp_pos = 13 + 9;
		break;
	case 'S':
		temp_pos = 10;
		break;
	case 'T':
		temp_pos = 13 + 10;
		break;
	case 'U':
		temp_pos = 11;
		break;
	case 'V':
		temp_pos = 13 + 11;
		break;
	case 'W':
		temp_pos = 12;
		break;
	case 'X':
		temp_pos = 13 + 12;
		break;
	case 'Y':
		temp_pos = 13;
		break;
	case 'Z':
		temp_pos = 13 + 13;
		break;
	}

	if(shiftFlag)
	{
		shiftFlag = 0;
	}
	FlashReadData((((unsigned long)(HOT_KEY_PG * PGBYTS)) + ((unsigned long)((temp_pos - 1) * HOT_KEY_LEN))), hot_key.arr, HOT_KEY_LEN);

	temp_code = hot_key.hot_key.product_code;

	return temp_code;
}
//-------------------------------------------------------------------------
unsigned char checkProductOnHotKey(unsigned char prodKey)
{
	UINT16 prodPosition = 0, loopProduct = 0;
	UCHAR exit = 0, keycheck = 0, i = 0;

	FlashReadData((((unsigned long)(HOT_KEY_PG * PGBYTS)) + ((unsigned long)((prodKey - 1) * HOT_KEY_LEN))), hot_key.arr, HOT_KEY_LEN);
	prodPosition = hot_key.hot_key.product_code;

	if(prodPosition <= MAX_PRODUCTS)
	{
		for(loopProduct = 0; (loopProduct <= mc.mc.prod_totalcount) && (!exit); loopProduct++)
		{
			FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)loopProduct * PRODUCT_LEN), membuf, PRODUCT_LEN);
			if(prodPosition == asc_hex(&membuf[PRODUCT_SER_NO], PRODUCT_SER_NO_LEN))
			{
				while(!exit)
				{
					clrscr();
					double_height = 0;
					double_width = 0;
					gotoxy(1,1);
					lcdprint("     SET PRODUCT    ");
					gotoxy(2,1);
					lcdprint("HOT KEY : ");
					if(hotKeyShift)
					{
						disp_char('S');
						disp_char('+');
					}
					disp_char(hotKey);
					gotoxy(3,1);
					lcdprint("PRODUCT NAME");
					double_height = 1;
					gotoxy(4,1);
					for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + 19); i++)
						disp_char(membuf[i]);
					double_height = 0;
					gotoxy(7,1);
					lcdprint("CNL-KEEP  ENT-CHANGE");
					gotoxy(8,1);
					lcdprint("PRINT-DELETE        ");

					while(!(keycheck = keyPressCheck()));

					switch(keycheck)
					{
						case '*':
									return 0;
						case '#':
									return 1;
						case PRINT_KEY:
									return 2;
						default:
									break;
					}
				}
				clrscr();
				return 1;
			}
			CheckOnOffKey();
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char paymentMode(void)
{
	unsigned char key = 0, exit = 0, screen_page = 0;

	screen_page = 1;
	while(!exit)
	{
		double_width = 0;
		double_height = 0;
		switch(screen_page)
		{
			case 1:
						clrscr();
						gotoxy(1,1);
						lcdprint("    PAYMENT MODE    ");
						gotoxy(2,1);
						lcdprint("1. CASH             ");
						gotoxy(3,1);
						lcdprint("2. CARD             ");
						gotoxy(4,1);
						lcdprint("3. PAYTM            ");
						gotoxy(5,1);
						lcdprint("4. GOOGLE PAY       ");
						gotoxy(6,1);
						lcdprint("5. PHONE PE         ");
						gotoxy(7,1);
						lcdprint("6. ZOMATO           ");
						gotoxy(8, 1);
						lcdprint("PG.1       v/ENT/CNL");
						break;
			case 2:
						clrscr();
						gotoxy(1,1);
						lcdprint("    PAYMENT MODE    ");
						gotoxy(2,1);
						lcdprint("7. SWIGGY           ");
						gotoxy(3,1);
						lcdprint("8. UBER EATS        ");
						gotoxy(4,1);
						lcdprint("9. FOOD PANDA       ");
						gotoxy(5,1);
						lcdprint("0. FAASOS           ");
						gotoxy(6,1);
						lcdprint("A. OTHERS           ");
						gotoxy(8, 1);
						lcdprint("PG.2       ^/ENT/CNL");
						break;
		}

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case  UP_ARROW:
						if(screen_page > 1)
							screen_page--;
						break;
			case  DOWN_ARROW:
						if(screen_page < 2)
							screen_page++;
						break;
			case '*':		//Mrugnil 10/09/24
						return 0xFF;		//Mrugnil 10/09/24
			case '1':
						return 1;
			case '2':
						return 2;
			case '3':
						return 3;
			case '4':
						return 4;
			case '5':
						return 5;
			case '6':
						return 6;
			case '7':
						return 7;
			case '8':
						return 8;
			case '9':
						return 9;
			case '0':
						return 0;
			case 'A':
			case 'B':
						return 11;
			default:
						break;
		}
	}

	return 1;
}
//-------------------------------------------------------------------------
void G1C_BattTest(void)
{
	unsigned char i = 0;
	unsigned char asc[16];
	unsigned char exit = 0, key = 0;

	clrscr();
	while(!exit)
	{
		double_height = 0;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("   BATTERY STATUS   ");

		ReadG1CBatt();
		gotoxy(3,1);
		lcdprint("BATT. VOLT: ");
		hex_asc(BATT_VOLTAGE, asc, 4);
		for(i = 0 ; i < 4 ; i++)
		{
			if(i == 1)
				disp_char('.');
			disp_char(asc[i]);
		}
		disp_char('V');

		gotoxy(4,1);
		lcdprint("CHRG. CURR: ");
		hex_asc(BATT_CURRENT, asc, 4);
		rem_zeros(asc, 3);
		for(i = 0 ; i < 4 ; i++)
			disp_char(asc[i]);
		lcdprint("mA");

//		gotoxy(5,1);
//		lcdprint("CHRG. VOLT: ");
//		hex_asc(USB_VOLTAGE, asc, 4);
//		rem_zeros(asc, 3);
//		for(i = 0 ; i < 4 ; i++)
//			disp_char(asc[i]);
//		lcdprint("V");

		gotoxy(8,1);
		lcdprint("PRESS            CNL");

		key = keyPressCheck();
		if(key == '*')
			exit = 1;
	}
	return;
}
//-------------------------------------------------------------------------
void ReadG1CBatt(void)
{
	unsigned char asc[16];
	unsigned char BattLevel = 0;
	unsigned char i, j, k, row = 8, col = 120, disp_width;
//	static unsigned char PrevBattLevel = 0xFF;
	static unsigned char ChargerConnected = 0xFF, ChargingStarted = 0xFF;

	criticalLowError = 0;

	if(updt_batt_flag == 1)
	{
		sci2_rx_completed = 0;
		sci2_tx_completed = 0;
		time_out = 0;
		R_SCI2_Serial_Send("#30@",4);
		while((sci2_tx_completed == 0) && (time_out < 1000));
		if(sci2_tx_completed)
		{
			time_out = 0;
			while((sci2_rx_completed == 0) && (time_out < 1000));
			if(sci2_rx_completed)
			{
				for(i = 3 ; i < 7 ; i++)
					asc[i - 3] = sci2_rx_buffer[i];
				BATT_VOLTAGE = asc_hex(asc, 4);
				for(i = 7 ; i < 10 ; i++)
					asc[i - 7] = sci2_rx_buffer[i];
				BATT_PERCENTAGE = asc_hex(asc, 3);
				for(i = 10 ; i < 14 ; i++)
					asc[i - 10] = sci2_rx_buffer[i];
				BATT_CURRENT = asc_hex(asc, 4);
				for(i = 14 ; i < 17 ; i++)
					asc[i - 14] = sci2_rx_buffer[i];
				USB_VOLTAGE = asc_hex(asc, 3);
			}
		}
		updt_batt_flag = 0;
	}
	lowBatteryError = 0;
#ifdef BATT_VIEW_PERCENTAGE
	if(BATT_PERCENTAGE >= 96)
		BattLevel = 8;
	else if((BATT_PERCENTAGE < 96) && (BATT_PERCENTAGE >= 84))
		BattLevel = 7;
	else if((BATT_PERCENTAGE < 84) && (BATT_PERCENTAGE >= 72))
		BattLevel = 6;
	else if((BATT_PERCENTAGE < 72) && (BATT_PERCENTAGE >= 60))
		BattLevel = 5;
	else if((BATT_PERCENTAGE < 60) && (BATT_PERCENTAGE >= 48))
		BattLevel = 4;
	else if((BATT_PERCENTAGE < 48) && (BATT_PERCENTAGE >= 36))
		BattLevel = 3;
	else if((BATT_PERCENTAGE < 36) && (BATT_PERCENTAGE >= 24))
		BattLevel = 2;
	else if((BATT_PERCENTAGE < 24) && (BATT_PERCENTAGE >= 12))
	{
		BattLevel = 1;
		lowBatteryError = 1;
	}
	else if(BATT_PERCENTAGE < 12)
	{
		BattLevel = 0;
		lowBatteryError = 1;
	}
#else
//	if(BATT_VOLTAGE >= 8250)
//		BattLevel = 8;
//	else if((BATT_VOLTAGE < 8250) && (BATT_VOLTAGE >= 8112))
//		BattLevel = 7;
//	else if((BATT_VOLTAGE < 8112) && (BATT_VOLTAGE >= 7925))
//		BattLevel = 6;
//	else if((BATT_VOLTAGE < 7925) && (BATT_VOLTAGE >= 7737))
//		BattLevel = 5;
//	else if((BATT_VOLTAGE < 7737) && (BATT_VOLTAGE >= 7550))
//		BattLevel = 4;
//	else if((BATT_VOLTAGE < 7550) && (BATT_VOLTAGE >= 7362))
//		BattLevel = 3;
//	else if((BATT_VOLTAGE < 7362) && (BATT_VOLTAGE >= 7175))
//		BattLevel = 2;
//	else if((BATT_VOLTAGE < 7175) && (BATT_VOLTAGE >= 6987))
//	{
//		BattLevel = 1;
//		lowBatteryError = 1;
//	}
//	else if(BATT_VOLTAGE < 6987)
//	{
//		BattLevel = 0;
//		lowBatteryError = 1;
//	}
	if(BATT_VOLTAGE >= 8100)
		BattLevel = 8;
	else if((BATT_VOLTAGE < 8100) && (BATT_VOLTAGE >= 7925))
		BattLevel = 7;
	else if((BATT_VOLTAGE < 7925) && (BATT_VOLTAGE >= 7750))
		BattLevel = 6;
	else if((BATT_VOLTAGE < 7750) && (BATT_VOLTAGE >= 7575))
		BattLevel = 5;
	else if((BATT_VOLTAGE < 7575) && (BATT_VOLTAGE >= 7400))
		BattLevel = 4;
	else if((BATT_VOLTAGE < 7400) && (BATT_VOLTAGE >= 7225))
		BattLevel = 3;
	else if((BATT_VOLTAGE < 7225) && (BATT_VOLTAGE >= 7000))
		BattLevel = 2;
	else if((BATT_VOLTAGE < 7000) && (BATT_VOLTAGE >= 6700))
	{
		BattLevel = 1;
		lowBatteryError = 1;
	}
	else if(BATT_VOLTAGE < 6700)
	{
		BattLevel = 0;
		lowBatteryError = 1;
		criticalLowError = 1;
	}
#endif
//	if(USB_VOLTAGE > 3)
	if(BATT_CURRENT > 0)
	{
		if(ChargingStarted != 1)
		{
			if(BATT_CURRENT != 0)
				ChargingStarted = 1;
		}

		if(ChargingStarted == 1)
		{
			if(BATT_CURRENT == 0)
				BatteryFull = 1;
		}

		if(ChargerConnected != 1)
		{
			blon();
			key_count = 0;
			ChargerConnected = 1;
		}

		StatusLedColor = LEDGRN;
		if(BattLevel >= 8)
			StatusLedFlash = 0;
		else
			StatusLedFlash = 1;

		if(!blink_flag)
			BattLevel = 8;
	}
	else
	{
		ChargingStarted = 0;
		BatteryFull = 0;

		if(ChargerConnected != 0)
		{
			blon();
			key_count = 0;
			ChargerConnected = 0;
		}

//		ChargeCtr = 0;

		if(BattLevel <= 0)
		{
			StatusLedColor = LEDRED;
			StatusLedFlash = 1;
		}
		else if(BattLevel == 1)
		{
			StatusLedColor = LEDRED;
			StatusLedFlash = 0;
		}
		else
		{
			StatusLedColor = LEDGRN;
			StatusLedFlash = 0;
		}
	}

	if(PrevBattLevel != BattLevel)
	{
		for(i = 0 ; i < 8 ; i++)
		{
			dispinst(0xB0 | i);
		  	dispinst((col >> 4)	| 0x10);		//	higher nibble of the col counter.
		  	dispinst((col & 0x0F)	| 0x00);	//	lower nibble of the col counter.
			for(j = 0 ; j < 8 ; j++)
				dispdata(0x00);
		}

		if(BattLevel)
		{
			for(i = 0 ; i < BattLevel ; i++)
			{
				row--;
				{
					dispinst(0xB0 | row);
				  	dispinst((col >> 4)	| 0x10);		//	higher nibble of the col counter.
				  	dispinst((col & 0x0F)	| 0x00);	//	lower nibble of the col counter.
					disp_width = ((8 - (row + 1))/2) + 1;
					for(k = 0 ; k < (8 - disp_width) ; k++)
						dispdata(0x00);
					for(k = 0 ; k < disp_width ; k++)
						dispdata(0x7F);
				}
			}
		}
		PrevBattLevel = BattLevel;
	}

}
//-------------------------------------------------------------------------
unsigned char chk_comm(void)
{
 	unsigned long wradd = 0, lastaddr = 0;
	unsigned char recv_flag = 1, exit = 0, prod_cnt = 0, unit_count = 0, productFileFlag = 0;
	unsigned char	discard = 0,data_err = 0,buf, appendProductDetailsMaster = 0, skipFirstLine = 0;
	unsigned long i = 0, pg_no = 10000, product_pos = 0;
	unsigned char agent = 0, send_recs = 0, break_loop = 0;
	unsigned char barcode_flag = 0,barcode_count = 0;
	int loop = 0, j = 0;//nilesh
	if(rec_flag == 1)
	{
		rec_flag = 0;
		productFileFlag = 0;
		clrscr();
		blon();
		double_height = 1;
		gotoxy(3, 1);
		lcdprint("  PLEASE WAIT ....");
		gotoxy(5, 1);
		lcdprint("  RECEIVING DATA !");
		double_height = 0;
		membuf[i++] = END_CHR;
		beep(SHRTBEEP);
		buf = 0;
		time_out1 = 0;
		while((buf == 0) && (time_out1 < 4999))
		{
			buf = pc_rd();
			if(!buf)
				pc_wrt('$');
		}
		membuf[i++] = buf;
		switch(buf)
		{
			case 'P':
						pg_no = PRODUCT_PG;
						lastaddr = PRODUCT_PG_END * PGBYTS;
						prod_cnt = 1;
						productFileFlag = 1;
						product_pos = 0;
						break;
			case 'U':
						pg_no	 = UNIT_TYPE_PG;
						lastaddr = UNIT_TYPE_PG_END * PGBYTS;
						unit_count = 1;
						break;
			case 'A':
						pg_no = AGENT_PG;
						lastaddr = AGENT_PG_END * PGBYTS;
						agent = 1;
						break;
			case 'D':
						send_recs = 1;
						exit = 1;
						recv_flag = 0;
						break;

			case 'B':
						send_recs = 2;
						exit = 1;
						recv_flag = 0;
						break;
			case 'E':
						exit = 1;
						recv_flag = 0;
						break;

			case 'H':
						pg_no = HEADER_PG;
						lastaddr = HEADER_PG_END * PGBYTS;
						break;
			case 'T':
						pg_no = CONFIG_PG;
						lastaddr = CONFIG_PG_END * PGBYTS;
						break;
			case 'F':
						pg_no    = IMAGE_PG;
						lastaddr = IMAGE_PG_END * PGBYTS;
						break;
			case 'I':
						pg_no = BARCODE_DETAILS_PG;
						lastaddr = BARCODE_DETAILS_PG_END * PGBYTS;
						barcode_flag = 1;
						barcode_count = 0;
						break;
			case 'M':
						pg_no = PRODUCT_PG;
						lastaddr = PRODUCT_PG_END * PGBYTS;
						prod_cnt = 1;
						appendProductDetailsMaster = 1;
						skipFirstLine = 1;
						wradd = (mc.mc.prod_totalcount * PRODUCT_LEN);
						product_pos = mc.mc.prod_totalcount;
						productFileFlag = 1;
						break;
			case 'C':
			case 'G':
			case 'J':
			case 'K':
			case 'L':
			case 'N':
			case 'O':
			case 'Q':
			case 'R':
			case 'S':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
						exit = 1;
						recv_flag = 0;
						break;
		 default:
						if(time_out1 >= 4999)
						{
							exit = 1;
							data_err = 1;
							recv_flag = 1;
						}
						//invalid command
						data_err = 1;
						recv_flag = 1;
						exit = 1;
		}

		if(!send_recs)
			pc_wrt('O');

		while(!exit)
		{
			break_loop = 0;
			time_out1 = 0;

			while(!break_loop)
			{
				buf=pc_rd();
				if(time_out1 > 499999)
				{
					break_loop = 1;
					exit = 1;
					data_err = 1;
				}

				if((buf==END_CHR)||(data_err))
				{
					exit=1;
					break_loop = 1;
				}

				if((buf=='@')||(exit))
				{
					break_loop = 1;
				}

				if(buf)
					membuf[i++]=buf;
				CheckOnOffKey();
			}

			if(((((unsigned long)pg_no * PGBYTS) + wradd) < lastaddr))
			{
				if(barcode_flag)
				{
					if(barcode_count > 0)
					{
						product_bacode_checksum = 0;
						memset(product_bacode,0,sizeof(product_bacode));
						strncpy(product_bacode,&membuf[PRODUCT_BARCODE],PRODUCT_BARCODE_LEN);
						strcat(product_bacode,"\0");
						for(loop = 0 ; loop < strlen(product_bacode) ; loop++)
							product_bacode_checksum += product_bacode[loop];
						store_barcode_details(barcode_count);
					}
					barcode_count++;
				}
				else if(!appendProductDetailsMaster)
				{
					FlashModifyData(((unsigned long)pg_no * PGBYTS) + wradd, membuf, i);
					if(productFileFlag)
					{
						for(j = PRODUCT_STOCK, loop = 0; j < (PRODUCT_STOCK + PRODUCT_STOCK_LEN); j++)
						{
							if(membuf[j] != '.')
								asc[loop++] = membuf[j];
						}

						product_stock.product_stock.totalStock = asc_hex(asc, (PRODUCT_STOCK_LEN - 1));
						product_stock.product_stock.sign = asc_hex(&membuf[PRODUCT_STOCK_SIGN], PRODUCT_STOCK_SIGN_LEN);
						store_product_stock(product_pos++);
					}
				}
				else if(appendProductDetailsMaster)
				{
					if(!skipFirstLine)
					{
						FlashModifyData(((unsigned long)pg_no * PGBYTS) + wradd, membuf, i);
						if(productFileFlag)
						{
							for(j = PRODUCT_STOCK, loop = 0; j < (PRODUCT_STOCK + PRODUCT_STOCK_LEN); j++)
							{
								if(membuf[j] != '.')
									asc[loop++] = membuf[j];
							}

							product_stock.product_stock.totalStock = asc_hex(asc, (PRODUCT_STOCK_LEN - 1));
							product_stock.product_stock.sign = asc_hex(&membuf[PRODUCT_STOCK_SIGN], PRODUCT_STOCK_SIGN_LEN);
							store_product_stock(product_pos++);
						}
					}
					skipFirstLine = 0;
				}
			}
			else
				data_err = 1;
			if(!discard)
				wradd += i;

			i = 0;
			discard = 0;

			pc_wrt('O');
			CheckOnOffKey();
		}
		if(recv_flag)
		{
			blon();
			clrscr();
			gotoxy(4, 1);
			if(data_err)
			{
				lcdprint("     DATA ERROR !");
				double_height = 0;
				gotoxy(7, 1);
				lcdprint("PLEASE SWITCH OFF ..");
				double_height = 1;
			}
			else
			{
				membuf[0] = '$';
				FlashModifyData(((unsigned long)pg_no * PGBYTS), membuf, 1);
				lcdprint("  DATA RECIEVED !");
				beep(OKKKBEEP);
			}
		}
		if(agent)
		{
			mc.mc.operator_totalcount = count_Agent();
			store_mc();
		}

		if(prod_cnt)
		{
			mc.mc.prod_totalcount = count_prod();
			store_mc(); 
			clearProductSummaryForSelectedDay(mc.mc.currentPG);
			clear_flash_for_barcode_details();
			prod_cnt = 0;
		}

		if(unit_count)
		{
			mc.mc.total_units = count_unit();
			store_mc();
			unit_count = 0;
		}

		if(send_recs)
		{
//			send_record_data(1);
			modemPushTimeOut = 0;
			stopPostCMDToModem();
			pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
			sendAllDataCMDToModem();

			send_recs = 0;
		}

		if(barcode_flag)
		{
			get_barcode_details(0);
			barcode_flag = 0;
		}

		store_mc();
		clrscr();
		blon();
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------------------
void send_record_data(unsigned char typeFlag)
{
	unsigned int i;
	UINT32 mainloop = 0, tempdata = 0, recs = 0;
	UCHAR pageSelect = 0, space_cnt = 0;

	const UCHAR orders[] = "MACHINE ID,DAY,STATUS,FIX STATUS,RECEIPT NO,PRODUCT CODE,PRODUCT NAME,RATE,QUANTITY,UNIT,PRODUCT AMOUNT,SGST AMOUNT,SGST RATE(%),CGST AMOUNT,CGST RATE(%),DISCOUNT AMOUNT,BILL STATUS,TYPE";
	const UCHAR bill[] = "MACHINE ID,DAY,STATUS,FIX STATUS,RECEIPT NO,DATE,TIME,SUB AMOUNT,SGST AMOUNT,DISCOUNT AMOUNT,CGST AMOUNT,OTHER CHARGES,OTHER CHARGES RATE(%),TOTAL AMOUNT,FINAL BILL AMOUNT,BILL STATUS,TYPE,PAYMENT MODE,CUSTOMER NAME, GSTIN";

	if(mc.mc.currentPG)
	{
		recs = mc.mc.recs;
		pageSelect = 1;
	}
	else
	{
		recs = mc.mc.rec1;
		pageSelect = 0;
	}
	if(typeFlag == 1)
	{
		for(i = 0; bill[i] != 0; i++)
			pc_wrt(bill[i]);
	}
	else
	{
		for(i = 0; orders[i] != 0; i++)
			pc_wrt(orders[i]);
	}
	pc_wrt('\n');

	FlashReadSecID();

	if(recs > 1)
	{
		clrscr();
		blon();
		double_width  = 0;
		double_height = 1;
		gotoxy(3, 1);
		lcdprint("SENDING RECORDS !  ");
		gotoxy(5, 1);
		lcdprint("DAY: CURRENT       ");
		beep(LONGBEEP);
		bloff();

		for(mainloop = 1; mainloop < recs; mainloop++)
		{
			get_rec_report(mainloop, pageSelect);

			if(rec.rec.last != 1)
			{
				if(typeFlag == 2)
				{
					for(i = MACHINE_ID_START; i < (MACHINE_ID_START + ETIMNO_LEN); i++)
						pc_wrt(SecID_Buffer[i]);
					pc_wrt(',');
					pc_wrt('C');
					pc_wrt(',');
					pc_wrt(rec.rec.strt_byt);
					pc_wrt(',');
					pc_wrt('S');
					pc_wrt(',');
					//RECEIPT NUMBER
					hex_asc(rec.rec.rcpt_no, asc, 5);
					for(i = 0; i < 5; i++)
						pc_wrt(asc[i]);
					pc_wrt(',');

					//PRODCUT CODE
					FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_SER_NO), membuf, PRODUCT_SER_NO_LEN);
					for(i = 0; i < PRODUCT_SER_NO_LEN; i++)
						pc_wrt(membuf[i]);
					pc_wrt(',');

					//PRODUCT NAME
					FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_TEXT), membuf, PRODUCT_TEXT_LEN);
					for(i = 0; i < PRODUCT_TEXT_LEN; i++)
						pc_wrt(membuf[i]);
					pc_wrt(',');

					//PRODUCT RATE
					hex_asc(rec.rec.rate, asc, 8);
					for(i = 0; i < 8; i++)
					{
						if(i == 6)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					hex_asc(rec.rec.quantity, asc, (mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot));
					rem_zeros(asc,mc.mc.digitsBeforeDot - 1);
					space_cnt = rem_zeros_frac(asc,mc.mc.digitsAfterDot,(mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot));
					if(space_cnt)
					{
						for(i = 0;i < mc.mc.digitsBeforeDot;i++)
							pc_wrt(asc[i]);
					}
					else
					{
						for(i = 0;i < (mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot);i++)
						{
							if(i == mc.mc.digitsBeforeDot)
								pc_wrt('.');
							pc_wrt(asc[i]);
						}
					}
					pc_wrt(',');

					if(rec.rec.unit_type)
					{
						//UNIT SYMBOL
						FlashReadData((((UINT32) UNIT_TYPE_PG * PGBYTS) + ((UINT32) rec.rec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), membuf, UNIT_TYPE_SYMBOL_LEN);
						for(i = 0; i < UNIT_TYPE_SYMBOL_LEN; i++)
							pc_wrt(membuf[i]);
						pc_wrt(',');
					}
					else
					{
						for(i = 0; i < UNIT_TYPE_SYMBOL_LEN; i++)
							pc_wrt(' ');
						pc_wrt(',');
					}

					//PRODUCT AMOUNT
					value_hex_asc_64U = rec.rec.final_prod_amount;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.sgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.prd_sgst, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.cgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.prd_cgst, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT DISCOUNT
					hex_asc(rec.rec.discount_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//VOID FLAG
					pc_wrt(rec.rec.bill_stat);
					pc_wrt(',');
					if(rec.rec.calc_stat)
						pc_wrt(rec.rec.calc_stat);
					else
						pc_wrt('0');

					pc_wrt('\n');
				}
			}
			else
			{
				if(typeFlag == 1)
				{
					for(i = MACHINE_ID_START; i < (MACHINE_ID_START + ETIMNO_LEN); i++)
						pc_wrt(SecID_Buffer[i]);
					pc_wrt(',');
					pc_wrt('C');
					pc_wrt(',');
					pc_wrt(rec.rec.strt_byt);
					pc_wrt(',');
					pc_wrt('L');
					pc_wrt(',');
					//RECEIPT NUMBER
					hex_asc(rec.rec.rcpt_no, asc, 5);
					for(i = 0; i < 5; i++)
						pc_wrt(asc[i]);
					pc_wrt(',');

					//DATE & TIME
					pk_unpk(rec.rec.dd, dd2);
					pk_unpk(rec.rec.mm, mm2);
					pk_unpk(rec.rec.yy, yy2);
					pk_unpk(rec.rec.hr, hr2);
					pk_unpk(rec.rec.mn, mn2);
					pk_unpk(rec.rec.sc, sc2);

					pc_wrt(dd2[0]);
					pc_wrt(dd2[1]);
					pc_wrt('/');
					pc_wrt(mm2[0]);
					pc_wrt(mm2[1]);
					pc_wrt('/');
					pc_wrt(yy2[0]);
					pc_wrt(yy2[1]);
					pc_wrt(',');
					pc_wrt(hr2[0]);
					pc_wrt(hr2[1]);
					pc_wrt(':');
					pc_wrt(mn2[0]);
					pc_wrt(mn2[1]);
					pc_wrt(':');
					pc_wrt(sc2[0]);
					pc_wrt(sc2[1]);
					pc_wrt(',');

					//FINAL AMOUNT
					value_hex_asc_64U = rec.rec.final_prod_amount;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//FINAL SGST
					hex_asc(rec.rec.sgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//FINAL DISCOUNT
					hex_asc(rec.rec.discount_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//CGST
					hex_asc(rec.rec.cgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.other_chrg, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.other_chrg_rate, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//TOTAL BILL AMOUNT
					value_hex_asc_64U = rec.rec.total_amt;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//FINAL BILL AMOUNT
					value_hex_asc_64U = rec.rec.round_off_tot_amt;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//VOID FLAG
					pc_wrt(rec.rec.bill_stat);
					pc_wrt(',');
					if(rec.rec.calc_stat)
						pc_wrt(rec.rec.calc_stat);
					else
						pc_wrt('0');
					pc_wrt(',');
					if(rec.rec.payment_mode == 1)
					{
						pc_wrt('C');
						pc_wrt('A');
						pc_wrt('S');
						pc_wrt('H');
					}
					else if(rec.rec.payment_mode == 2)
					{
						pc_wrt('C');
						pc_wrt('A');
						pc_wrt('R');
						pc_wrt('D');
					}
					else if(rec.rec.payment_mode == 3)
					{
						pc_wrt('P');
						pc_wrt('A');
						pc_wrt('Y');
						pc_wrt('T');
						pc_wrt('M');
					}
					else if(rec.rec.payment_mode == 4)
					{
						pc_wrt('G');
						pc_wrt('P');
						pc_wrt('A');
						pc_wrt('Y');
					}else if(rec.rec.payment_mode == 5)
					{
						pc_wrt('P');
						pc_wrt('H');
						pc_wrt('O');
						pc_wrt('N');
						pc_wrt('E');
						pc_wrt('P');
						pc_wrt('E');
					}
					else if(rec.rec.payment_mode == 6)
					{
						pc_wrt('Z');
						pc_wrt('O');
						pc_wrt('M');
						pc_wrt('A');
						pc_wrt('T');
						pc_wrt('O');
					}else if(rec.rec.payment_mode == 7)
					{
						pc_wrt('S');
						pc_wrt('W');
						pc_wrt('I');
						pc_wrt('G');
						pc_wrt('G');
						pc_wrt('Y');
					}
					else if(rec.rec.payment_mode == 8)
					{
						pc_wrt('U');
						pc_wrt('B');
						pc_wrt('E');
						pc_wrt('R');
						pc_wrt(' ');
						pc_wrt('E');
						pc_wrt('A');
						pc_wrt('T');
						pc_wrt('S');
					}
					else if(rec.rec.payment_mode == 9)
					{
						pc_wrt('F');
						pc_wrt('O');
						pc_wrt('O');
						pc_wrt('D');
						pc_wrt(' ');
						pc_wrt('P');
						pc_wrt('A');
						pc_wrt('N');
						pc_wrt('D');
						pc_wrt('A');
					}
					else if(rec.rec.payment_mode == 0)
					{
						pc_wrt('F');
						pc_wrt('A');
						pc_wrt('A');
						pc_wrt('S');
						pc_wrt('O');
						pc_wrt('S');
					}
					else if(rec.rec.payment_mode == 11)
					{
						pc_wrt('O');
						pc_wrt('T');
						pc_wrt('H');
						pc_wrt('E');
						pc_wrt('R');
						pc_wrt('S');
					}
					if(mc.mc.cust_details_rpt)
					{
						if(mc.mc.cust_name_entry)
						{
							if(rec.rec.cust_detail_pos > 0)
							{
								pc_wrt(',');
								get_cust_details(rec.rec.cust_detail_pos, pageSelect);

								if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_CUST_NAME))
								{
									for(i = 0; i < CUSTOMER_NAME_LEN; i++)
										pc_wrt(cust_details.cust_details.CUSTOMER_NAME[i]);
									pc_wrt(',');
								}
								if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_GSTIN))
								{
									for(i = 0; i < GSTIN_NO_LEN; i++)
										pc_wrt(cust_details.cust_details.GSTIN[i]);
								}
							}
						}
					}
					pc_wrt('\n');
				}
			}
		}
	}
	else
	{
		clrscr();
		gotoxy(3,1);
		double_width = 0;
		double_height = 1;
		lcdprint("  NO RECORDS FOUND  ");
		gotoxy(5,1);
		lcdprint("   IN CURRENT DAY   ");
		beep(ERRRBEEP);
		Delay_ms(1000);
	}

	if(mc.mc.currentPG)
	{
		recs = mc.mc.rec1;
		pageSelect = 0;
	}
	else
	{
		recs = mc.mc.recs;
		pageSelect = 1;
	}

	if(recs > 1)
	{
		clrscr();
		blon();
		double_width  = 0;
		double_height = 1;
		gotoxy(3, 1);
		lcdprint("SENDING RECORDS !  ");
		gotoxy(5, 1);
		lcdprint("DAY: PREVIOUS      ");
		beep(LONGBEEP);
		bloff();

		for(mainloop = 1; mainloop < recs; mainloop++)
		{
			get_rec_report(mainloop, pageSelect);

			if(rec.rec.last != 1)
			{
				if(typeFlag == 2)
				{
					for(i = MACHINE_ID_START; i < (MACHINE_ID_START + ETIMNO_LEN); i++)
						pc_wrt(SecID_Buffer[i]);
					pc_wrt(',');
					pc_wrt('P');
					pc_wrt(',');
					pc_wrt(rec.rec.strt_byt);
					pc_wrt(',');
					pc_wrt('S');
					pc_wrt(',');
					//RECEIPT NUMBER
					hex_asc(rec.rec.rcpt_no, asc, 5);
					for(i = 0; i < 5; i++)
						pc_wrt(asc[i]);
					pc_wrt(',');

					//PRODCUT CODE
					FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_SER_NO), membuf, PRODUCT_SER_NO_LEN);
					for(i = 0; i < PRODUCT_SER_NO_LEN; i++)
						pc_wrt(membuf[i]);
					pc_wrt(',');

					//PRODUCT NAME
					FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_TEXT), membuf, PRODUCT_TEXT_LEN);
					for(i = 0; i < PRODUCT_TEXT_LEN; i++)
						pc_wrt(membuf[i]);
					pc_wrt(',');

					//PRODUCT RATE
					hex_asc(rec.rec.rate, asc, 8);
					for(i = 0; i < 8; i++)
					{
						if(i == 6)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					hex_asc(rec.rec.quantity, asc, (mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot));
					rem_zeros(asc,mc.mc.digitsBeforeDot - 1);
					space_cnt = rem_zeros_frac(asc,mc.mc.digitsAfterDot,(mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot));
					if(space_cnt)
					{
						for(i = 0;i < mc.mc.digitsBeforeDot;i++)
							pc_wrt(asc[i]);
					}
					else
					{
						for(i = 0;i < (mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot);i++)
						{
							if(i == mc.mc.digitsBeforeDot)
								pc_wrt('.');
							pc_wrt(asc[i]);
						}
					}
					pc_wrt(',');

					if(rec.rec.unit_type)
					{
						//UNIT SYMBOL
						FlashReadData((((UINT32) UNIT_TYPE_PG * PGBYTS) + ((UINT32) rec.rec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), membuf, UNIT_TYPE_SYMBOL_LEN);
						for(i = 0; i < UNIT_TYPE_SYMBOL_LEN; i++)
							pc_wrt(membuf[i]);
						pc_wrt(',');
					}
					else
					{
						for(i = 0; i < UNIT_TYPE_SYMBOL_LEN; i++)
							pc_wrt(' ');
						pc_wrt(',');
					}

					//PRODUCT AMOUNT
					value_hex_asc_64U = rec.rec.final_prod_amount;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.sgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.prd_sgst, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.cgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.prd_cgst, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT DISCOUNT
					hex_asc(rec.rec.discount_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//VOID FLAG
					pc_wrt(rec.rec.bill_stat);
					pc_wrt(',');
					if(rec.rec.calc_stat)
						pc_wrt(rec.rec.calc_stat);
					else
						pc_wrt('0');

					pc_wrt('\n');
				}
			}
			else
			{
				if(typeFlag == 1)
				{
					for(i = MACHINE_ID_START; i < (MACHINE_ID_START + ETIMNO_LEN); i++)
						pc_wrt(SecID_Buffer[i]);
					pc_wrt(',');
					pc_wrt('P');
					pc_wrt(',');
					pc_wrt(rec.rec.strt_byt);
					pc_wrt(',');
					pc_wrt('L');
					pc_wrt(',');
					//RECEIPT NUMBER
					hex_asc(rec.rec.rcpt_no, asc, 5);
					for(i = 0; i < 5; i++)
						pc_wrt(asc[i]);
					pc_wrt(',');

					//DATE & TIME
					pk_unpk(rec.rec.dd, dd2);
					pk_unpk(rec.rec.mm, mm2);
					pk_unpk(rec.rec.yy, yy2);
					pk_unpk(rec.rec.hr, hr2);
					pk_unpk(rec.rec.mn, mn2);
					pk_unpk(rec.rec.sc, sc2);

					pc_wrt(dd2[0]);
					pc_wrt(dd2[1]);
					pc_wrt('/');
					pc_wrt(mm2[0]);
					pc_wrt(mm2[1]);
					pc_wrt('/');
					pc_wrt(yy2[0]);
					pc_wrt(yy2[1]);
					pc_wrt(',');
					pc_wrt(hr2[0]);
					pc_wrt(hr2[1]);
					pc_wrt(':');
					pc_wrt(mn2[0]);
					pc_wrt(mn2[1]);
					pc_wrt(':');
					pc_wrt(sc2[0]);
					pc_wrt(sc2[1]);
					pc_wrt(',');

					//FINAL AMOUNT
					value_hex_asc_64U = rec.rec.final_prod_amount;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//FINAL SGST
					hex_asc(rec.rec.sgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//FINAL DISCOUNT
					hex_asc(rec.rec.discount_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//CGST
					hex_asc(rec.rec.cgst_amt, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.other_chrg, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//PRODUCT SGST
					hex_asc(rec.rec.other_chrg_rate, asc, 9);
					for(i = 0; i < 9; i++)
					{
						if(i == 7)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//TOTAL BILL AMOUNT
					value_hex_asc_64U = rec.rec.total_amt;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//FINAL BILL AMOUNT
					value_hex_asc_64U = rec.rec.round_off_tot_amt;
					hex_asc_64U(asc, 12);
					for(i = 0; i < 12; i++)
					{
						if(i == 10)
							pc_wrt('.');
						pc_wrt(asc[i]);
					}
					pc_wrt(',');

					//VOID FLAG
					pc_wrt(rec.rec.bill_stat);
					pc_wrt(',');
					if(rec.rec.calc_stat)
						pc_wrt(rec.rec.calc_stat);
					else
						pc_wrt('0');
					pc_wrt(',');
					if(rec.rec.payment_mode == 1)
					{
						pc_wrt('C');
						pc_wrt('A');
						pc_wrt('S');
						pc_wrt('H');
					}
					else if(rec.rec.payment_mode == 2)
					{
						pc_wrt('C');
						pc_wrt('A');
						pc_wrt('R');
						pc_wrt('D');
					}
					else if(rec.rec.payment_mode == 3)
					{
						pc_wrt('P');
						pc_wrt('A');
						pc_wrt('Y');
						pc_wrt('T');
						pc_wrt('M');
					}
					else if(rec.rec.payment_mode == 4)
					{
						pc_wrt('G');
						pc_wrt('P');
						pc_wrt('A');
						pc_wrt('Y');
					}else if(rec.rec.payment_mode == 5)
					{
						pc_wrt('P');
						pc_wrt('H');
						pc_wrt('O');
						pc_wrt('N');
						pc_wrt('E');
						pc_wrt('P');
						pc_wrt('E');
					}
					else if(rec.rec.payment_mode == 6)
					{
						pc_wrt('Z');
						pc_wrt('O');
						pc_wrt('M');
						pc_wrt('A');
						pc_wrt('T');
						pc_wrt('O');
					}else if(rec.rec.payment_mode == 7)
					{
						pc_wrt('S');
						pc_wrt('W');
						pc_wrt('I');
						pc_wrt('G');
						pc_wrt('G');
						pc_wrt('Y');
					}
					else if(rec.rec.payment_mode == 8)
					{
						pc_wrt('U');
						pc_wrt('B');
						pc_wrt('E');
						pc_wrt('R');
						pc_wrt(' ');
						pc_wrt('E');
						pc_wrt('A');
						pc_wrt('T');
						pc_wrt('S');
					}
					else if(rec.rec.payment_mode == 9)
					{
						pc_wrt('F');
						pc_wrt('O');
						pc_wrt('O');
						pc_wrt('D');
						pc_wrt(' ');
						pc_wrt('P');
						pc_wrt('A');
						pc_wrt('N');
						pc_wrt('D');
						pc_wrt('A');
					}
					else if(rec.rec.payment_mode == 0)
					{
						pc_wrt('F');
						pc_wrt('A');
						pc_wrt('A');
						pc_wrt('S');
						pc_wrt('O');
						pc_wrt('S');
					}
					else if(rec.rec.payment_mode == 11)
					{
						pc_wrt('O');
						pc_wrt('T');
						pc_wrt('H');
						pc_wrt('E');
						pc_wrt('R');
						pc_wrt('S');
					}

					if(mc.mc.cust_details_rpt)
					{
						if(mc.mc.cust_name_entry)
						{
							if(rec.rec.cust_detail_pos > 0)
							{
								pc_wrt(',');
								get_cust_details(rec.rec.cust_detail_pos, pageSelect);

								if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_CUST_NAME))
								{
									for(i = 0; i < CUSTOMER_NAME_LEN; i++)
										pc_wrt(cust_details.cust_details.CUSTOMER_NAME[i]);
									pc_wrt(',');
								}
								if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_GSTIN))
								{
									for(i = 0; i < GSTIN_NO_LEN; i++)
										pc_wrt(cust_details.cust_details.GSTIN[i]);
								}
							}
						}
					}
					pc_wrt('\n');
				}
			}
		}
	}
	else
	{
		clrscr();
		gotoxy(3,1);
		double_width = 0;
		double_height = 1;
		lcdprint("  NO RECORDS FOUND  ");
		gotoxy(5,1);
		lcdprint("   IN CURRENT DAY   ");
		beep(ERRRBEEP);
		Delay_ms(1000);
	}

	pc_wrt(0x04);

	clrscr();
	gotoxy(3,1);
	double_width = 0;
	double_height = 1;
	lcdprint("   ALL DATA SENT!   ");
	beep(OKKKBEEP);
	Delay_ms(4000);
}
//------------------------------------------------------------------------
void prt_bar(void)
{
	UCHAR barLoop = 0, switcher = 0;		//Mrugnil 09/05/24
	setLanguage(1);

//	lineBarPrinting = 1;		//Mrugnil 09/05/24
//
////	unicode_prt_buf("- - - - - - - - - - - - - - - - - - - - - - ");//44
////	unicode_prt_buf("- - - - - - - - - - - - - - - - - - - - - - - - ");//48		//Mrugnil 12/02/24
////	unicode_printD(CC);
////	unicode_prt_buf("----------------------------------");
//	for(barLoop = 0, switcher = 0; barLoop < 56; barLoop++)		//Mrugnil 09/05/24
//	{
//		if(!switcher)
//		{
//			unicode_prnbuf(0x81);		//Mrugnil 09/05/24
//			switcher = 1;
//		}
//		else
//		{
//			unicode_prnbuf('');		//Mrugnil 09/05/24
//			switcher = 0;
//		}
//	}
//	unicode_printD(CC);
//	lineBarPrinting = 0;		//Mrugnil 09/05/24

	unicode_prt_buf("- - - - - - - - - - - - - - - - - - - - - - - - ");//48		//Mrugnil 12/02/24
	unicode_print(DH);
}
//-------------------------------------------------------------------------
unsigned char  printType(void)
{
	UCHAR exit = 0, key = 0;

	if((mc.mc.udf_en_dis) && (mc.mc.kot_en_dis) && (!returnBill))
	{
		clrscr();
		double_height = 0;
		gotoxy(1,1);
		lcdprint("    PRINT OPTION  ");
		double_height = 0;
		gotoxy(2,1);
		lcdprint("1. PRINT KOT      ");
		gotoxy(3,1);
		lcdprint("2. BILL           ");
		gotoxy(4,1);
		lcdprint("3. SAVE KOT       ");
		gotoxy(5,1);
		lcdprint("4. EDIT KOT       ");
		gotoxy(6,1);
		lcdprint("5. VIEW BILL AMT  ");
		gotoxy(7,1);
		lcdprint("6. BILL SAVE      ");
		gotoxy(8,1);
		lcdprint("PRESS   CHS/CANCEL");

		while(!exit)
		{
			key = 0;
			while(!(key = keyPressCheck()));
			switch(key)
			{
				case '1':
							return 1;
				case '2':
							return 2;
				case '3':
							return 3;
				case '4':
							return 4;
				case '5':
							return 5;
				case '6':
							return 6;
				case '*':
							return 0;
				default :
							break;
			}
		}
	}
	else if((!returnBill) && (mc.mc.retail_bill_opt))
	{
		clrscr();
		double_height = 0;
		gotoxy(1,1);
		lcdprint("    PRINT OPTION  ");
		double_height = 0;
		gotoxy(2,1);
		lcdprint("1. PRINT          ");
		gotoxy(3,1);
		lcdprint("2. SAVE BILL      ");
		gotoxy(8,1);
		lcdprint("PRESS   CHS/CANCEL");

		while(!exit)
		{
			key = 0;
			while(!(key = keyPressCheck()));
			switch(key)
			{
				case '1':
							return 2;
				case '2':
							return 6;
				case '*':
							return 0;
				default :
							break;
			}
		}
	}
	else
		return 2;

	return 0;
}
//------------------------------------------------------------------------
void disp_printmsg(void)
{
	clrscr();
	gotoxy(4,1);
	lcdprint("     PRINTING ....");
	double_height=0;
	gotoxy(7,1);
	lcdprint("    Please Wait !");
	double_height=1;
	bloff();
}
//-------------------------------------------------------------------------
void homeScreen(void)
{
	UCHAR count = 0, buff[UDF_NAME_LEN];

	spaceFlag = 0;

	if(mc.mc.gprs_en_dis || mc.mc.gsm_en_dis)
	{
		double_height = 0;
		double_width = 0;

		if(MODEM_BUSY != 1)
		{
			if(tout2_simCheck > 60000)
			{
				simSTAT = check_sim_card_exists();
				tout2_simCheck = 0;
			}
			if(tout2_disp_network > 15000)
			{
				check_signal_strength();
				tout2_disp_network = 0;
				checkBusy();
			}

		}
		else
		{
			if(tout2_disp_network > 9999)
			{
				checkBusy();
				tout2_disp_network = 0;
			}
		}
		networkDisp(1,1,signal_strength_index);
	}

	if(loginFlag)
	{
		gotoxy(2,1);
		if(lowBatteryError)
		{
			double_height = 0;
			double_width = 0;
			gotoxy(1,5);
			lcdprint("BBP SUPER STAR");
			double_height = 0;
			double_width = 0;
			gotoxy(2,1);
			lcdprint("    LOW BATTERY!    ");
		}
		else
		{
			double_height = 0;
			double_width = 0;
			gotoxy(1,5);
			lcdprint("BBP SUPER STAR");
			double_height = 0;
			double_width = 0;
			gotoxy(2,1);
			lcdprint("                    ");
		}
		double_height = 0;
		double_width = 0;
		gotoxy(1,20);
		if(shiftFlag)
			disp_char('S');
		else
			disp_char(' ');

		double_height = 1;
		double_width = 0;
		gotoxy(4,1);
		if((!udf_no) && (mc.mc.udf_en_dis)&&(!returnBill))
		{
			FlashReadData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME), buff, UDF_NAME_LEN);
			for(count = 0; count < 10; count++)
			{
				disp_char(buff[count]);
			}
			disp_char(':');
			disp_char(' ');
			for(count = 0; count < 5; count++)
			{
				disp_char(' ');
			}
		}
		else
		{
			if((mc.mc.udf_en_dis))
			{
				double_height = 0;
				double_width = 0;
				gotoxy(3,1);
				get_rate_slab_name(slab_no);
				if(slab_name.slab_name.set == 'Y')
				{
					if((!mc.mc.parcel_en_dis) || ((mc.mc.parcel_en_dis == 1) && (udf_no < 226)))
					{
						get_rate_slab_name(slab_no);
						memset(asc, 0, sizeof(asc));
						centerAlignInBuff(slab_name.slab_name.name, asc, 20);
						lcdprint(asc);
					}
				}

				gotoxy(4,1);
				FlashReadData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME), buff, UDF_NAME_LEN);
				for(count = 0; count < 10; count++)
				{
					disp_char(buff[count]);
				}
				get_table_map(udf_no); //vikas for tablemap
				hex_asc(tablemap.tablemap.table_map, asc, 4);
				rem_zeros(asc, 3);
				disp_char(':');
				disp_char(' ');
				if(udf_no)
				{
					for(count = 0; count < 4; count++)
						disp_char(asc[count]);
				}
				else
				{
					for(count = 0; count < 3; count++)
						disp_char(' ');
				}

				gotoxy(7,1);
				lcdprint("AMT : ");
				value_hex_asc_64U = selectedAmount;
				hex_asc_64U(asc, 11);
				rem_zeros(asc, 7);
				if(selectedAmount > 0)
				{
					for(count = 0; count < 11; count++)
					{
						if(count == 9)
							disp_char('.');
						disp_char(asc[count]);
					}
				}
				else
				{
					if(last_bill_amount && mc.mc.last_amt_display_en_dis)
					{
						value_hex_asc_64U = last_bill_amount;
						hex_asc_64U(asc, 11);
						rem_zeros(asc, 7);
						for(count = 0; count < 11; count++)
						{
							if(count == 9)
								disp_char('.');
							disp_char(asc[count]);
						}
					}
				}
			}
			else
			{
				if(returnBill)
				{
					double_height = 0;
					double_width = 0;
					gotoxy(3,1);
					lcdprint("  RETURN BILL MODE  ");
				}
				else
				{
					double_height = 0;
					double_width = 0;
					gotoxy(3,1);
					clrRow(3);
				}
			}

			double_height = 0;
			double_width = 0;
			gotoxy(5,1);
			lcdprint("I: ");
			hex_asc(selectedProd, asc, 3);
			rem_zeros(asc, 3);
			for(count = 0; count < 3; count++)
			{
				disp_char(asc[count]);
			}

			if(selectedQty)
			{
				disp_char(' ');
				disp_char('Q');
				disp_char(':');
				hex_asc(selectedQty, asc, 9);
				rem_zeros(asc, 6);
				for(count = 0; count < 9; count++)
				{
					if(count == (9 - mc.mc.digitsAfterDot))
						disp_char('.');
					disp_char(asc[count]);
				}
			}

			double_height = 0;
			gotoxy(7,1);
			lcdprint("AMT : ");
			value_hex_asc_64U = selectedAmount;
			hex_asc_64U(asc, 11);
			rem_zeros(asc, 7);
			if(selectedAmount > 0)
			{
				for(count = 0; count < 11; count++)
				{
					if(count == 9)
						disp_char('.');
					disp_char(asc[count]);
				}
			}
			else
			{
				if(last_bill_amount && mc.mc.last_amt_display_en_dis)
				{
					value_hex_asc_64U = last_bill_amount;
					hex_asc_64U(asc, 11);
					rem_zeros(asc, 7);
					for(count = 0; count < 11; count++)
					{
						if(count == 9)
							disp_char('.');
						disp_char(asc[count]);
					}
				}
			}
		}
		disp_rtc(8);
	}
	else
	{
		login();
	}
}
//-------------------------------------------------------------------------
void centerAlignInBuff(unsigned char * actualBuf, unsigned char * modifiedBuf, unsigned char LineLen)
{
	UCHAR temp_buf[UDF_NAME_LEN + 2], temp_buf1[UDF_NAME_LEN + 2], i = 0, buffer[50];
	UCHAR space_count = 0, offset_spacing = 0, char_count = 0, localPadding = 0;

	memset(temp_buf, 0, sizeof(temp_buf));
	memset(temp_buf1, 0, sizeof(temp_buf1));
	memset(buffer, 0, sizeof(buffer));
	memset(buffer, ' ', LineLen);

	strncpy(temp_buf, actualBuf, UDF_NAME_LEN);

	for(i = 0, space_count = 0; i < UDF_NAME_LEN; i++)
	{
		if(((temp_buf[i] == ' ') && (temp_buf[i + 1] == ' ')) || ((temp_buf[i] != ' ') && (temp_buf[i + 1] == ' ') && (temp_buf[i + 2] == ' ')))
		{
			space_count++;
		}
		else
			char_count++;
	}

	if(LineLen == 48)
		offset_spacing = 14;
	else if(LineLen == 32)
		offset_spacing = 6;
	else
		offset_spacing = 0;

	if(space_count > 0)
		localPadding = (space_count / 2);
	else
		localPadding = 0;

	for(i = 0; i <= localPadding; i++)
	{
		temp_buf1[i] = ' ';
	}

	for(i = localPadding; i < (localPadding + char_count); i++)
	{
		temp_buf1[i] = temp_buf[i - localPadding];
	}

	for(i = offset_spacing; i < (offset_spacing + strlen(temp_buf1)); i++)
	{
		buffer[i] = temp_buf1[i - offset_spacing];
	}

	strncpy(modifiedBuf, buffer, LineLen);

	i = 0;
}
//-------------------------------------------------------------------------
unsigned int udf_entry(unsigned int entryIndex)
{
	UCHAR key, tempbuf[5] = {0}, i = 0;
	UINT16 exit = 0, temp_udf = 0, udf_dig = 0, char_count = 0;

	memset(tempbuf, ' ', 4);

	udf_dig = 4;

	tempbuf[temp_udf++] = entryIndex;

	while(!exit)
	{
		double_height = 0;
		double_width  = 0;
		gotoxy(1,5);
		lcdprint("BBP SUPER STAR");

		double_height = 1;
		double_width  = 0;
		gotoxy(4,1);
		FlashReadData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME), membuf, UDF_NAME_LEN);
		for(i = 0; i < 10; i++)
		{
			disp_char(membuf[i]);
		}

		disp_char(':');
		disp_char(' ');

		for(i = 0; i < udf_dig; i++)
			disp_char(tempbuf[i]);

		double_height = 0;
		gotoxy(8,1);
		lcdprint("PRESS:       ENT/CNL");
		double_height = 1;

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case BACK_SPACE:
							if(temp_udf > 0)
							{
								tempbuf[--temp_udf] = ' ';
								if(char_count)
								{
									udf_char = 0;
									char_count = 0;
								}
							}
							entryIndex = asc_hex(tempbuf, temp_udf);
							break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
						if((temp_udf < udf_dig))
						{
							tempbuf[temp_udf++] = key;
						}
						entryIndex = asc_hex(tempbuf, temp_udf);
						break;
			case '*':
						udf_char = 0;
						return 0;
			case '#':
						entryIndex = asc_hex(tempbuf, temp_udf);

						entryIndex = Map_udf_entry(entryIndex);

						if(mc.mc.parcel_en_dis)
						{
							if(((entryIndex <= mc.mc.max_table) && (entryIndex > 0)) || ((entryIndex > 225 ) && (entryIndex <= 250))) //vikas
							{
								get_udf(entryIndex);
								if(udfno.udfno.udfActive)
								{
									clrscr();
									double_height = 1;
									double_width = 0;
									gotoxy(3,1);
									lcdprint("    ORDER IS OPEN   ");
									beep(SHRTBEEP);
									Delay_ms(500);
								}
								else
								{
									clrscr();
									double_height = 1;
									double_width = 0;
									gotoxy(3,1);
									lcdprint("  NEW ORDER STARTED ");
									beep(SHRTBEEP);
									Delay_ms(500);
								}
								return entryIndex;
							}
							else if((entryIndex > mc.mc.max_table) || (entryIndex == 0))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(2,1);
								lcdprint("   ENTERED UDF NO.  ");
								gotoxy(4,1);
								lcdprint("   NOT CONFIGURED   ");
								beep(ERRRBEEP);
								Delay_ms(2000);
								clrscr();
								entryIndex = 0;
								memset(tempbuf, ' ', 4);
								temp_udf = 0;
							}
						}
						else if((entryIndex <= mc.mc.max_table) && (entryIndex > 0)) //vikas
						{
							get_udf(entryIndex);
							if(udfno.udfno.udfActive)
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(3,1);
								lcdprint("    ORDER IS OPEN   ");
								beep(SHRTBEEP);
								Delay_ms(500);
							}
							else
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(3,1);
								lcdprint("  NEW ORDER STARTED ");
								beep(SHRTBEEP);
								Delay_ms(500);
							}
							return entryIndex;
						}
						else if((entryIndex > mc.mc.max_table) || (entryIndex == 0))//vikas
						{
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(2,1);
							lcdprint("   ENTERED UDF NO.  ");
							gotoxy(4,1);
							lcdprint("   NOT CONFIGURED   ");
							beep(ERRRBEEP);
							Delay_ms(2000);
							clrscr();
							entryIndex = 0;
							memset(tempbuf, ' ', 4);
							temp_udf = 0;
						}
						break;
			default:
						break;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
void loadConfigDetails(void)
{
	UCHAR i = 0, buffer[CONFIG_LEN], count = 0;

	memset(buffer, ' ', CONFIG_LEN);

	FlashReadData(((UINT32) CONFIG_PG * PGBYTS), buffer, CONFIG_LEN);

	for(i = CONFIG_UDF_NAME; i < (CONFIG_UDF_NAME + UDF_NAME_LEN); i++)
		config.config.udf_name[i - CONFIG_UDF_NAME] = buffer[i];

	for(i = CONFIG_OTHER_CHARGES_NAME; i < (CONFIG_OTHER_CHARGES_NAME + OTHER_CHARGES_NAME_LEN); i++)
		config.config.other_chrg_name[i - CONFIG_OTHER_CHARGES_NAME] = buffer[i];

	for(i = CONFIG_OTHER_CHARGES_RATE, count = 0; i < (CONFIG_OTHER_CHARGES_RATE + OTHER_CHARGES_RATE_LEN); i++)
	{
		if(buffer[i] != '.')
			asc[count++] = buffer[i];
	}

	config.config.other_chrg_rate = asc_hex(asc, (OTHER_CHARGES_RATE_LEN - 1));

	config.config.other_charges = asc_hex(&buffer[CONFIG_OTHER_CHARGES_FLAG], CONFIG_FLAG_LEN);
	config.config.udf_en = asc_hex(&buffer[CONFIG_UDF_FLAG], CONFIG_FLAG_LEN);
	config.config.kot_en = asc_hex(&buffer[CONFIG_KOT_FLAG], CONFIG_FLAG_LEN);
	config.config.kot_en_panel = asc_hex(&buffer[CONFIG_KOT_PANEL_FLAG], CONFIG_FLAG_LEN);
	config.config.cash_recp_en = asc_hex(&buffer[CONFIG_CASH_RECEPTION_FLAG], CONFIG_FLAG_LEN);
	config.config.manual_rate_en = asc_hex(&buffer[CONFIG_MANUAL_RATE_FLAG], CONFIG_FLAG_LEN);
	config.config.max_tables = asc_hex(&buffer[CONFIG_MAX_TABLES], MAX_TABLES_LEN);
	config.config.roundoff_typ = asc_hex(&buffer[CONFIG_ROUNDOFF_FLAG], CONFIG_FLAG_LEN);
	config.config.discount_typ = asc_hex(&buffer[CONFIG_DISCOUNT_FLAG], CONFIG_FLAG_LEN);
	config.config.billType = asc_hex(&buffer[CONFIG_BILL_NO_TYPE_FLAG], CONFIG_FLAG_LEN);

	mc.mc.kot_en_dis = config.config.kot_en;
	mc.mc.udf_en_dis = config.config.udf_en;
	mc.mc.round_off = config.config.roundoff_typ;
	mc.mc.discount_type = config.config.discount_typ;
	if(!mc.mc.discount_type)
		mc.mc.discount_en = 0;
	mc.mc.cash_reception_enable = config.config.cash_recp_en;
	mc.mc.modify_rate_enable = config.config.manual_rate_en;
	mc.mc.max_table = config.config.max_tables;
	mc.mc.billNoType = config.config.billType;
	store_mc();
}
//-------------------------------------------------------------------------
void clear_global_variable(void)
{
	UCHAR loopcount = 0;

	for(loopcount = 0; loopcount < RECDAT_LEN; loopcount++)
		rec.arr[loopcount] = 0;

	countSI = 0;
	memset(tempArr,' ',5);
	product_code = 0;
	final_bill_amount = 0;
	qty = 0;
	prod_discount = 0;
	prod_vat = 0;
	unit_type = 0;
	unit_fract = 0;
	pre_roundoff = 0;
	post_roundoff = 0;
	recevied_cash = 0;
	credit_amount = 0;
	returned_amount = 0;
	hotKeyConfigureFlag = 0;
	hotKey = 0;
	hotKeyShift = 0;
	localAmount = 0;
	memset(g_customerName,' ',CUSTOMER_NAME_LEN);
	memset(g_GST_IN,' ',GSTIN_NO_LEN);
	pretax_discount_rupee = 0;
}
//-------------------------------------------------------------------------
unsigned char getHotKeyProductDetails(unsigned int index)
{
	UINT32 prodPosition = 0;
	UCHAR found = 0, loopCount = 0, storeCount = 0;
	prodPosition = productBinarySearch(index);
//	if(prodPosition > 0)
	if((prodPosition > 0) && (!deletedProduct))		//Mrugnil 27/06/24
	{
		found = 1;
	}
	else if((prodPosition > 0) && (deletedProduct == 1))		//Mrugnil 27/06/24
	{
		deletedProduct = 0;		//Mrugnil 27/06/24
		found = 0;		//Mrugnil 27/06/24
	}
	else
		found = 0;

	if(found)
	{
		FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);

		switch(slab_no)
		{
			case 1:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT_LEN - 1);
					break;
			case 2:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT2_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT2 + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT2 + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT2_LEN - 1);
					break;
			case 3:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT3_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT3 + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT3 + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT3_LEN - 1);
					break;
		}
		//get product discount
		for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_DISCOUNT_LEN; loopCount++)
		{
			if(membuf[PRODUCT_DISCOUNT + loopCount] != '.')
			{
				asc[storeCount++] = membuf[PRODUCT_DISCOUNT + loopCount];
			}
		}
		prod_discount = asc_hex(asc, PRODUCT_DISCOUNT_LEN - 1);

		//get product SGST
		for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_SGST_LEN; loopCount++)
		{
			if(membuf[PRODUCT_SGST + loopCount] != '.')
			{
				asc[storeCount++] = membuf[PRODUCT_SGST + loopCount];
			}
		}
		prod_vat = asc_hex(asc, PRODUCT_SGST_LEN - 1);

		product_code = prodPosition;

		unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));

		return 1;
	}
	else
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(3,1);
		lcdprint("  INVALID HOT KEY!  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		return 0;
	}
}
//-------------------------------------------------------------------------
unsigned char searchProduct(unsigned int entryIndex, unsigned char usedInEditingFunction)		//Mrugnil 27/06/24
{
	UINT32 j,loopCount, storeCount, prodPosition = 0, scrollStart = 0,tempUTFchar=0, tempPos = 0;
	UINT8 key, LINE_LEN = 0;
	BIT exit = 0, shiftFlag = 0, found = 0, scrollFlag = 0;

	Vikas_Timer = 0;

	scrollStart = 1;
	while(!exit)
	{
		found = 0;
		deletedProduct = 0;		//Mrugnil 27/06/24

		if(barcode_entry_flag == 1)
		{
			prodPosition = search_for_duplicate_barcode_details(0);
			if(prodPosition > 0)
				found = 1;
			else
			{
				clrscr();
				gotoxy(5,1);
				lcdprint("   ITEM NOT FOUND   ");
				beep(ERRRBEEP);
				return 0;
			}
		}
		else
		{
			if(scrollFlag)
			{
				prodPosition = scrollStart;
				FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);
				if(membuf[PRODUCT_EN_FLAG] != 1)
					found = 1;
				else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
				{
					found = 1;
					deletedProduct = 1;		//Mrugnil 20/06/24
				}
				else
					found = 0;
			}
			else
			{
				prodPosition = productBinarySearch(entryIndex);
				if((prodPosition > 0) && ((!usedInEditingFunction) && (!deletedProduct)))		//Mrugnil 27/06/24
				{
					found = 1;
					scrollStart = prodPosition;
				}
				else if((prodPosition > 0) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
				{
					found = 1;
					scrollStart = prodPosition;
				}
				else
					found = 0;
			}
		}

		if(found)
		{
			FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);

			clrscr();
			if(hotKeyConfigureFlag)
			{
				double_height = 0;
				double_width = 1;
				gotoxy(1,1);
				lcdprint("   ITEMS  ");
				double_width = 0;
				gotoxy(2,1);
				lcdprint("HOT KEY : ");
				if(hotKeyShift)
				{
					disp_char('S');
					disp_char('+');
				}
				disp_char(hotKey);
			}
			else
			{

				double_height = 1;
				double_width = 1;
				gotoxy(1,1);
				lcdprint("   ITEMS  ");

			}

			double_height = 0;
			double_width = 0;
			gotoxy(3,1);
			for(loopCount = 0; loopCount < PRODUCT_SER_NO_LEN; loopCount++)
			{
				disp_char(membuf[PRODUCT_SER_NO + loopCount]);
			}

			gotoxy(4,1);
			//print the product Name
//			if(!deletedProduct)		//Mrugnil 27/06/24
//			{
//				for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN - 7; loopCount++)
//				{
//					disp_char(membuf[PRODUCT_TEXT + loopCount]);
//				}
//			}
//			else
//				lcdprint("                    ");		//Mrugnil 27/06/24

			if(!deletedProduct)		//Mrugnil 10/01/25
			{
				for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN; loopCount++)		//Mrugnil 10/01/25
				{
					disp_char(membuf[PRODUCT_TEXT + loopCount]);		//Mrugnil 10/01/25
				}
			}
			else		//Mrugnil 10/01/25
			{
				for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN; loopCount++)		//Mrugnil 10/01/25
				{
					disp_char(' ');		//Mrugnil 10/01/25
				}
			}		//Mrugnil 10/01/25

			double_height = 1;
			double_width = 0;
			gotoxy(6,1);

			switch(slab_no)
			{
				case 1:
						for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT_LEN; loopCount++)
						{
							if(membuf[PRODUCT_AMOUNT + loopCount] != '.')
							{
								asc[storeCount++] = membuf[PRODUCT_AMOUNT + loopCount];
							}
						}
						final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT_LEN - 1);
						break;
				case 2:
						for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT2_LEN; loopCount++)
						{
							if(membuf[PRODUCT_AMOUNT2 + loopCount] != '.')
							{
								asc[storeCount++] = membuf[PRODUCT_AMOUNT2 + loopCount];
							}
						}
						final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT2_LEN - 1);
						break;
				case 3:
						for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT3_LEN; loopCount++)
						{
							if(membuf[PRODUCT_AMOUNT3 + loopCount] != '.')
							{
								asc[storeCount++] = membuf[PRODUCT_AMOUNT3 + loopCount];
							}
						}
						final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT3_LEN - 1);
						break;

			}

			rem_zeros(asc, 5);
			if(!deletedProduct)		//Mrugnil 27/06/24
			{
				for(loopCount = 0; loopCount < (PRODUCT_AMOUNT_LEN - 1); loopCount++)
				{
					if(loopCount == 6)
						disp_char('.');
					disp_char(asc[loopCount]);
				}
			}
			else
				lcdprint("   DELETED PRODUCT  ");		//Mrugnil 27/06/24

			//get product discount
			for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_DISCOUNT_LEN; loopCount++)
			{
				if(membuf[PRODUCT_DISCOUNT + loopCount] != '.')
				{
					asc[storeCount++] = membuf[PRODUCT_DISCOUNT + loopCount];
				}
			}
			prod_discount = asc_hex(asc, PRODUCT_DISCOUNT_LEN - 1);

			//get product SGST
			for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_SGST_LEN; loopCount++)
			{
				if(membuf[PRODUCT_SGST + loopCount] != '.')
				{
					asc[storeCount++] = membuf[PRODUCT_SGST + loopCount];
				}
			}
			prod_vat = asc_hex(asc, PRODUCT_SGST_LEN - 1);

			product_code = prodPosition;

			unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));

			FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
			unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);

			double_width = 0;
			double_height = 0;
			gotoxy(8,1);
			if(!shiftFlag)
				lcdprint("         v/^/ENT/CNL");
			else
				lcdprint("SHIFT    v/^/ENT/CNL");
		}
		else
		{
			clrscr();

			double_height = 1;
			double_width = 1;
			gotoxy(1,1);
			lcdprint("   ITEMS  ");

			found = 0;
			double_height = 0;
			double_width = 0;
			gotoxy(3,1);
			hex_asc(entryIndex, asc, 4);
			for(loopCount = 0; loopCount < 4; loopCount++)
				disp_char(asc[loopCount]);

			gotoxy(5,1);
			lcdprint("   ITEM NOT FOUND   ");
			double_width = 0;
			double_height = 0;
			gotoxy(8,1);
			if(!shiftFlag)
				lcdprint("         v/^/ENT/CNL");
			else
				lcdprint("SHIFT    v/^/ENT/CNL");
		}

		scrollFlag = 0;

		if(barcode_entry_flag == 1)
		{
			if(found)
			{
				if(mc.mc.barcode_qty_en_dis == 1) //vikas
				{
					return 1;
				}
				else
				{
					return 4;
				}
			}

			else
				return 0;
		}
		else
		{
			while(!(key = keyPressCheck()));

			switch(key)
			{
				case DOWN_ARROW:
							if(scrollStart < mc.mc.prod_totalcount)
							{
								tempPos = scrollStart++;
								found = 0;
								while((!found) && (scrollStart <= mc.mc.prod_totalcount))
								{
									FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
									if(membuf[PRODUCT_EN_FLAG] != 1)
										found = 1;
									else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
									{
										found = 1;
										deletedProduct = 1;		//Mrugnil 20/06/24
									}
									else
										scrollStart++;

								}
								if(!found)
								{
									scrollStart--;
									while((!found) && (scrollStart > 1))
									{
										FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
										if(membuf[PRODUCT_EN_FLAG] != 1)
											found = 1;
										else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
										{
											found = 1;
											deletedProduct = 1;		//Mrugnil 20/06/24
										}
										else
											scrollStart--;
									}
								}
								if(!found)
									scrollStart = tempPos;
								scrollFlag = 1;
							}
							else
							{
								tempPos = scrollStart;
								scrollFlag = 1;
								scrollStart = 1;
								found = 0;
								while((!found) && (scrollStart <= mc.mc.prod_totalcount))
								{
									FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
									if(membuf[PRODUCT_EN_FLAG] != 1)
										found = 1;
									else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
									{
										found = 1;
										deletedProduct = 1;		//Mrugnil 20/06/24
									}
									else
										scrollStart++;

								}
								if(!found)
									scrollStart = tempPos;
								scrollFlag = 1;
							}
							break;
				case BACK_SPACE:

							if(shiftFlag)
							{
								if(countSI > 0)
									countSI--;
								if(countSI >= 0)
									tempArr[countSI] = ' ';
								if(countSI <= 0)
								{
									countSI = 0;
									tempArr[countSI] = ' ';
								}
								entryIndex = asc_hex(tempArr, countSI);
								scrollStart = 1;
							}
							else
							{
								if(scrollStart > 1)
								{
									tempPos = scrollStart--;
									found = 0;

									while((!found) && (scrollStart > 0))
									{
										FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
										if(membuf[PRODUCT_EN_FLAG] != 1)
											found = 1;
										else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
										{
											found = 1;
											deletedProduct = 1;		//Mrugnil 20/06/24
										}
										else
											scrollStart--;
									}

									if(!found)
									{
										scrollStart++;
										while((!found) && (scrollStart <= mc.mc.prod_totalcount))
										{
											FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
											if(membuf[PRODUCT_EN_FLAG] != 1)
												found = 1;
											else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
											{
												found = 1;
												deletedProduct = 1;		//Mrugnil 20/06/24
											}
											else
												scrollStart++;

										}
									}
									if(!found)
										scrollStart = tempPos;
									scrollFlag = 1;
								}
								else
								{
									if(mc.mc.prod_totalcount > 1)
									{
										scrollStart = mc.mc.prod_totalcount;
										scrollFlag = 1;
										found = 0;

										while((!found) && (scrollStart > 0))
										{
											FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
											if(membuf[PRODUCT_EN_FLAG] != 1)
												found = 1;
											else if((membuf[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction == 1))		//Mrugnil 27/06/24
											{
												found = 1;
												deletedProduct = 1;		//Mrugnil 20/06/24
											}
											else
												scrollStart--;
										}
										if(!found)
											scrollStart = tempPos;
										scrollFlag = 1;
									}
								}
							}
							break;
				case SHIFT_KEY:
							shiftFlag = ~shiftFlag;
							break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					        if((Vikas_Timer > 3000))
					        {
					        	countSI = 0;
					        	memset(tempArr, '\0', sizeof(tempArr));
					        	Vikas_Timer = 0;
					        }
							if(countSI < 4)
								tempArr[countSI++] = key;
							entryIndex = asc_hex(tempArr, countSI);
							scrollStart = 1;
							break;
				case '*':
							return 0;
				case PRINT_KEY:
							if(mc.mc.product_selection == BEFORE_QTY)
							{
								if(!recArrayPtr && !returnBill && searchProductSingle)
								{
									if(found)
									{
										oneProductSale(product_code);
										return 2;
									}
								}
							}
							break;
				case MENU_KEY:
							if(!mc.mc.udf_en_dis)
							{
								if((scrollStart <= mc.mc.prod_totalcount) && (scrollStart >= 1) && (found))
									return 3;
							}
							break;
				case '#':
							if((scrollStart <= mc.mc.prod_totalcount) && (scrollStart >= 1) && (found))
								return 1;
							break;
			}
		}
	}
	
	deletedProduct = 0;		//Mrugnil 27/06/24
	
	return 0;
}
//-------------------------------------------------------------------------
unsigned long productBinarySearch(unsigned long productCodeUI32)
{
	UINT32 low = 0, high = 0, mid = 0;
	UINT32 tempproductCodeUI32 = 0;
	UCHAR result=0, buff[PRODUCT_LEN + 1];

	low = 1;
	high = mc.mc.prod_totalcount;

	do
	{
		deletedProduct = 0;		//Mrugnil 27/06/24
		mid= (low + high) / 2;
		FlashReadData(((UINT32)PRODUCT_PG * PGBYTS) + ((UINT32)mid * PRODUCT_LEN) + PRODUCT_SER_NO, buff, PRODUCT_SER_NO_LEN + 1);
		tempproductCodeUI32 = asc_hex(buff, PRODUCT_SER_NO_LEN);
		if((productCodeUI32 == tempproductCodeUI32))
		{
			if(buff[PRODUCT_EN_FLAG] != 1)
				result = 1;
			else if(buff[PRODUCT_EN_FLAG] == 1)		//Mrugnil 27/06/24
			{
				result = 1;
				deletedProduct = 1;		//Mrugnil 27/06/24
			}
			else
				result = 0;
			break;
		}
		else
		{
			if(productCodeUI32 < tempproductCodeUI32)
			{
			  high = mid - 1;
			}
			if(productCodeUI32 > tempproductCodeUI32)
			{
			  low = mid + 1;
			}
		}
	}while((!result) && (low <= high));

	if(result)
		return mid;
	else
		return 0;
}
//-------------------------------------------------------------------------
void oneProductSale(unsigned int productPosition)
{
	unsigned int i = 0;
	unsigned char exit = 0;
	UINT32 records = 0, lastAdd = 0;

	UINT32 product_stock_checker = 0;		//Mrugnil 30/01/24
	UCHAR product_sign_checker = 0;		//Mrugnil 30/01/24

	if(mc.mc.currentPG)
		records = mc.mc.recs;
	else
		records = mc.mc.rec1;

	FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)productPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);
	FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
	unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);

//	FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)product_code * PRODUCT_LEN), asc, PRODUCT_LEN);
//	for(i = PRODUCT_STOCK_SIGN; i < PRODUCT_STOCK_SIGN_LEN; i++)		//Mrugnil 30/01/24
//		product_sign_checker = membuf[i];
//	product_stock_checker = asc_hex(&asc[PRODUCT_STOCK], PRODUCT_STOCK_LEN);		//Mrugnil 30/01/24
	product_sign_checker = temp_product_stock[product_code].temp_product_stock.sign;		//Mrugnil 30/01/24
	product_stock_checker = temp_product_stock[product_code].temp_product_stock.totalStock;		//Mrugnil 30/01/24

	if(mc.mc.qty_enter_method == QTY_3_3)
		qty = 1000;
	else
		qty = 100;
	if(!final_bill_amount)
	{
		allow_rate_sel = 1;
		enter_manual_rate(final_bill_amount);
		allow_rate_sel = 0;
	}

	if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
	{
		if(((!mc.mc.stock_minus)&&(!product_stock_checker))||((!mc.mc.stock_minus)&&(product_sign_checker)))		//Mrugnil 30/01/24
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3, 1);
			lcdprint(" STOCK IS LESS THAN ");
			gotoxy(5, 1);
			lcdprint("  ENTERED QUANTITY  ");
			beep(ERRRBEEP);		//Mrugnil 30/01/24
			Delay_ms(1000);
			double_height = 0;
			double_width = 0;
			clrscr();		//Mrugnil 30/01/24
		}
		else
		{
			store_entry(0);
			clear_global_variable();
			print_mode = printType();

			switch(print_mode)
			{
				case 1:
						//print KOT
						saveOrder();
						if(selectedProd > 0)
							KOTprint();
						recArrayPtr = 0;
						udf_no = 0;
						break;
				case 2:
						//print BILL
						if(mc.mc.paymode && (!returnBill))
						{
							paymentType = paymentMode();
						}
						else
						{
							paymentType = 1;
						}
						if(selectedProd > 0)
							saveOrder();
						if(mc.mc.cust_name_entry)
						{
							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_CUST_NAME))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER CUSTOMER NAME ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
							}

							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_GSTIN))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER GSTIN NO:   ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_GST_IN, GSTIN_NO_LEN, 1, 0);
							}
						}
						if(store_last_rec())
						{
							recArrayPtr = 0;
							loadOrderDetailsIntoRAM();

							if(mc.mc.currentPG)
								lastAdd = mc.mc.recs;
							else
								lastAdd = mc.mc.rec1;

							closeOpenBill(startRec, lastAdd, 0);

							if(mc.mc.cash_reception_enable && (!returnBill))
							{
								if(rec.rec.payment_mode == 1)
								{
									while(!exit)
									{
										clrscr();
										double_height = 0;
										gotoxy(1,1);
										lcdprint("   CASH RECEPTION   ");

										gotoxy(8,1);
										lcdprint("             CNL/ENT");
										double_height = 1;

										gotoxy(3,1);
										lcdprint("BILL AMT: ");
										hex_asc(rec.rec.round_off_tot_amt, asc, 9);
										rem_zeros(asc, 5);
										for(i = 0; i < 9; i++)
										{
											if(i == 7)
												disp_char('.');
											disp_char(asc[i]);
										}
										gotoxy(6,1);
										lcdprint("PAY AMT: ");


										recordedDotPosition = 0;
										get_dig_dot_esc_key = 0;
										recevied_cash = get_dig_dot(6,10,7,2,0,0);
										if(get_dig_dot_esc_key)
										{
											recevied_cash = 0;
											returned_amount = 0;
											credit_amount = rec.rec.round_off_tot_amt;
											exit = 1;
										}
										else
										{
											if(recevied_cash < rec.rec.round_off_tot_amt)
											{
												credit_amount = (rec.rec.round_off_tot_amt - recevied_cash);
												exit = 1;
											}
											else
											{
												returned_amount = (recevied_cash - rec.rec.round_off_tot_amt);
												exit = 1;
											}
										}
									}
								}
							}
							if(mc.mc.udf_en_dis)
							{
								get_udf(udf_no);
								udfno.udfno.udfActive = 0;
								udfno.udfno.udfStart = 0;
								udfno.udfno.udfRcptNo = 0;
								store_udf(udf_no);

								get_udf_prd_cnt(udf_no);
								udf_prd_cnt.udf_prd_cnt.prd_count = 0;
								store_udf_prd_cnt(udf_no);
							}
							if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
							{
								if(mc.mc.rcpt_no >= MAX_RCPTNO)
									mc.mc.rcpt_no = 1;
								else
									mc.mc.rcpt_no++;
							}
							store_mc();

							if(mc.mc.udf_en_dis)
								bill_print();
							else
							{
								if(mc.mc.bill_format_selection == TAX_SORTING)
								{
									printNonUDFBill();

//		//							if((mc.mc.coupon_en_dis) && (!mc.mc.udf_en_dis) && (!mc.mc.kot_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//									if((mc.mc.coupon_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//									{
//										if(yes_no("   PRINT  COUPON?   "))
//											couponprint();
//									}
									if((mc.mc.coupon_en_dis) && (!returnBill))		//Mrugnil 30/01/24
									{
										if(yes_no("   PRINT  COUPON?   "))		//Mrugnil 30/01/24
											couponprint();
									}
								}
								else
									bill_print();
							}

							if(mc.mc.drawer_en_dis) //VIKAS
							{
								DRW_ENABLE = 1;
								Delay_ms(200);
								DRW_ENABLE = 0;
							}

							mc.mc.rec_start = 0;
							mc.mc.selected = 0;
							store_mc();
							selectedAmount = 0;
							selectedQty = 0;
							selectedProd = 0;
							recArrayPtr = 0;
							returnBill = 0;

							if(mc.mc.gprs_en_dis)
							{
								stopPostCMDToModem();
								pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
							}
							if(mc.mc.gsm_en_dis == RECORD_LOT)
							{
								if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
								{
									sendDayWiseSummary_SMSToAllMobileNumbers();
								}
							}
							udf_no = 0;
						}
						else
						{
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(3,1);
							lcdprint("   NO BILL FOUND!   ");
							beep(ERRRBEEP);
							Delay_ms(3000);
							break;
						}
						break;
				case 4:
						//edit KOT
						if((recArrayPtr > 0) && (selectedProd > 0))
						{
							editOrderFromUnsavedKot();
						}
						else
							editOrder();
						if((recArrayPtr > 0) && (selectedProd > 0))
						{
							if(yes_no("    SAVE & PRINT?   "))
							{
								saveOrder();
								KOTprint();
								selectedAmount = 0;
								selectedQty = 0;
								selectedProd = 0;
								recArrayPtr = 0;
								udf_no = 0;
							}
						}
						break;
				case 3:
						if(selectedProd)
						{
							saveOrder();
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(3,1);
							lcdprint("     KOT  SAVED     ");
							beep(LONGBEEP);
							Delay_ms(1000);
							recArrayPtr = 0;
							udf_no = 0;
						}
						break;
				case 5:
						viewBillAmount(udf_no);
						break;
				case 6:
						//print BILL
						if(mc.mc.paymode && (!returnBill))
						{
							paymentType = paymentMode();
						}
						else
						{
							paymentType = 1;
						}
						if(selectedProd > 0)
							saveOrder();
						if(mc.mc.cust_name_entry)
						{
							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_CUST_NAME))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER CUSTOMER NAME ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
							}

							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_GSTIN))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER GSTIN NO:   ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_GST_IN, GSTIN_NO_LEN, 1, 0);
							}
						}
						if(store_last_rec())
						{
							if(mc.mc.currentPG)
								records = mc.mc.recs;
							else
								records = mc.mc.rec1;

							closeOpenBill(startRec, records, 0);

							if(mc.mc.udf_en_dis)
							{
								get_udf(udf_no);
								udfno.udfno.udfActive = 0;
								udfno.udfno.udfStart = 0;
								udfno.udfno.udfRcptNo = 0;
								store_udf(udf_no);

								get_udf_prd_cnt(udf_no);
								udf_prd_cnt.udf_prd_cnt.prd_count = 0;
								store_udf_prd_cnt(udf_no);
							}

//		//					if((mc.mc.coupon_en_dis) && (!mc.mc.udf_en_dis) && (!mc.mc.kot_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//							if((mc.mc.coupon_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//							{
//								if(yes_no("   PRINT  COUPON?   "))
//									couponprint();
//							}
							if((mc.mc.coupon_en_dis) && (!returnBill))		//Mrugnil 30/01/24
							{
								if(yes_no("   PRINT  COUPON?   "))		//Mrugnil 30/01/24
									couponprint();
							}

							if(mc.mc.drawer_en_dis)
							{
								DRW_ENABLE = 1;
								Delay_ms(200);
								DRW_ENABLE = 0;
							}

							mc.mc.rec_start = 0;
							mc.mc.selected = 0;
							selectedAmount = 0;
							selectedQty = 0;
							selectedProd = 0;
							recArrayPtr = 0;
							returnBill = 0;
							if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
							{
								if(mc.mc.rcpt_no >= MAX_RCPTNO)
									mc.mc.rcpt_no = 1;
								else
									mc.mc.rcpt_no++;
							}
							store_mc();
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(3,1);
							lcdprint("     BILL SAVED     ");
							Delay_ms(1000);
							if(mc.mc.gprs_en_dis)
							{
								stopPostCMDToModem();
								pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
							}
							if(mc.mc.gsm_en_dis == RECORD_LOT)
							{
								if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
								{
									sendDayWiseSummary_SMSToAllMobileNumbers();
								}
							}
							udf_no = 0;
						}
						break;
				default:
						break;
			}
		}
	}
	else		//Mrugnil 12/02/24
	{
		if(((!mc.mc.stock_minus)&&(!product_stock_checker))||((!mc.mc.stock_minus)&&(product_sign_checker)))		//Mrugnil 12/02/24
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3, 1);
			lcdprint(" STOCK IS LESS THAN ");
			gotoxy(5, 1);
			lcdprint("  ENTERED QUANTITY  ");
			beep(ERRRBEEP);		//Mrugnil 12/02/24
			Delay_ms(1000);
			double_height = 0;
			double_width = 0;
			clrscr();		//Mrugnil 12/02/24
		}
		else
		{
			store_entry(0);
			clear_global_variable();
			print_mode = printType();

			switch(print_mode)
			{
				case 1:
						//print KOT
						saveOrder();
						if(selectedProd > 0)
							KOTprint();
						recArrayPtr = 0;
						udf_no = 0;
						break;
				case 2:
						//print BILL
						if(mc.mc.paymode && (!returnBill))
						{
							paymentType = paymentMode();
						}
						else
						{
							paymentType = 1;
						}
						if(selectedProd > 0)
							saveOrder();
						if(mc.mc.cust_name_entry)
						{
							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_CUST_NAME))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER CUSTOMER NAME ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
							}

							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_GSTIN))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER GSTIN NO:   ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_GST_IN, GSTIN_NO_LEN, 1, 0);
							}
						}
						if(store_last_rec())
						{
							recArrayPtr = 0;
							loadOrderDetailsIntoRAM();

							if(mc.mc.currentPG)
								lastAdd = mc.mc.recs;		//Mrugnil 12/02/24
							else
								lastAdd = mc.mc.rec1;

							closeOpenBill(startRec, lastAdd, 0);

							if(mc.mc.cash_reception_enable && (!returnBill))
							{
								if(rec.rec.payment_mode == 1)
								{
									while(!exit)
									{
										clrscr();
										double_height = 0;
										gotoxy(1,1);
										lcdprint("   CASH RECEPTION   ");

										gotoxy(8,1);
										lcdprint("             CNL/ENT");
										double_height = 1;

										gotoxy(3,1);
										lcdprint("BILL AMT: ");
										hex_asc(rec.rec.round_off_tot_amt, asc, 9);
										rem_zeros(asc, 5);
										for(i = 0; i < 9; i++)
										{
											if(i == 7)
												disp_char('.');
											disp_char(asc[i]);
										}
										gotoxy(6,1);
										lcdprint("PAY AMT: ");


										recordedDotPosition = 0;
										get_dig_dot_esc_key = 0;
										recevied_cash = get_dig_dot(6,10,7,2,0,0);
										if(get_dig_dot_esc_key)
										{
											recevied_cash = 0;
											returned_amount = 0;
											credit_amount = rec.rec.round_off_tot_amt;		//Mrugnil 12/02/24
											exit = 1;
										}
										else
										{
											if(recevied_cash < rec.rec.round_off_tot_amt)
											{
												credit_amount = (rec.rec.round_off_tot_amt - recevied_cash);
												exit = 1;
											}
											else
											{
												returned_amount = (recevied_cash - rec.rec.round_off_tot_amt);
												exit = 1;
											}
										}
									}
								}
							}
							if(mc.mc.udf_en_dis)
							{
								get_udf(udf_no);
								udfno.udfno.udfActive = 0;
								udfno.udfno.udfStart = 0;
								udfno.udfno.udfRcptNo = 0;
								store_udf(udf_no);

								get_udf_prd_cnt(udf_no);
								udf_prd_cnt.udf_prd_cnt.prd_count = 0;
								store_udf_prd_cnt(udf_no);
							}
							if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
							{
								if(mc.mc.rcpt_no >= MAX_RCPTNO)
									mc.mc.rcpt_no = 1;
								else
									mc.mc.rcpt_no++;
							}
							store_mc();

							if(mc.mc.udf_en_dis)
								bill_print();
							else
							{
								if(mc.mc.bill_format_selection == TAX_SORTING)
								{
									printNonUDFBill();

//		//							if((mc.mc.coupon_en_dis) && (!mc.mc.udf_en_dis) && (!mc.mc.kot_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//									if((mc.mc.coupon_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//									{
//										if(yes_no("   PRINT  COUPON?   "))
//											couponprint();
//									}
									if((mc.mc.coupon_en_dis) && (!returnBill))		//Mrugnil 12/02/24
									{
										if(yes_no("   PRINT  COUPON?   "))		//Mrugnil 12/02/24
											couponprint();
									}
								}
								else
									bill_print();
							}

							if(mc.mc.drawer_en_dis) //VIKAS
							{
								DRW_ENABLE = 1;
								Delay_ms(200);
								DRW_ENABLE = 0;
							}

							mc.mc.rec_start = 0;
							mc.mc.selected = 0;
							store_mc();
							selectedAmount = 0;
							selectedQty = 0;
							selectedProd = 0;
							recArrayPtr = 0;
							returnBill = 0;

							if(mc.mc.gprs_en_dis)
							{
								stopPostCMDToModem();
								pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
							}
							if(mc.mc.gsm_en_dis == RECORD_LOT)
							{
								if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
								{
									sendDayWiseSummary_SMSToAllMobileNumbers();
								}
							}
							udf_no = 0;
						}
						else
						{
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(3,1);
							lcdprint("   NO BILL FOUND!   ");
							beep(ERRRBEEP);
							Delay_ms(3000);
							break;
						}
						break;
				case 4:
						//edit KOT
						if((recArrayPtr > 0) && (selectedProd > 0))
						{
							editOrderFromUnsavedKot();
						}
						else
							editOrder();
						if((recArrayPtr > 0) && (selectedProd > 0))
						{
							if(yes_no("    SAVE & PRINT?   "))
							{
								saveOrder();
								KOTprint();
								selectedAmount = 0;
								selectedQty = 0;
								selectedProd = 0;
								recArrayPtr = 0;
								udf_no = 0;
							}
						}
						break;
				case 3:
						if(selectedProd)
						{
							saveOrder();
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(3,1);
							lcdprint("     KOT  SAVED     ");
							beep(LONGBEEP);
							Delay_ms(1000);
							recArrayPtr = 0;
							udf_no = 0;
						}
						break;
				case 5:
						viewBillAmount(udf_no);
						break;
				case 6:
						//print BILL
						if(mc.mc.paymode && (!returnBill))
						{
							paymentType = paymentMode();
						}
						else
						{
							paymentType = 1;
						}
						if(selectedProd > 0)
							saveOrder();
						if(mc.mc.cust_name_entry)
						{
							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_CUST_NAME))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER CUSTOMER NAME ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
							}

							if((mc.mc.cust_name_entry == CUST_BOTH)
									|| (mc.mc.cust_name_entry == O_GSTIN))
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(1,1);
								lcdprint("ENTER GSTIN NO:   ");
								double_height = 0;
								gotoxy(8,1);
								lcdprint("PRESS        ENT/CNL");
								double_height = 1;

								getAlphaDig(4, 1, g_GST_IN, GSTIN_NO_LEN, 1, 0);
							}
						}
						if(store_last_rec())
						{
							if(mc.mc.currentPG)
								records = mc.mc.recs;
							else
								records = mc.mc.rec1;

							closeOpenBill(startRec, records, 0);

							if(mc.mc.udf_en_dis)
							{
								get_udf(udf_no);
								udfno.udfno.udfActive = 0;
								udfno.udfno.udfStart = 0;
								udfno.udfno.udfRcptNo = 0;
								store_udf(udf_no);

								get_udf_prd_cnt(udf_no);
								udf_prd_cnt.udf_prd_cnt.prd_count = 0;
								store_udf_prd_cnt(udf_no);
							}

//		//					if((mc.mc.coupon_en_dis) && (!mc.mc.udf_en_dis) && (!mc.mc.kot_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//							if((mc.mc.coupon_en_dis) && (!mc.mc.twoCopyBill)&&(!returnBill))
//							{
//								if(yes_no("   PRINT  COUPON?   "))
//									couponprint();
//							}
							if((mc.mc.coupon_en_dis) && (!returnBill))		//Mrugnil 12/02/24
							{
								if(yes_no("   PRINT  COUPON?   "))		//Mrugnil 12/02/24
									couponprint();
							}

							if(mc.mc.drawer_en_dis)
							{
								DRW_ENABLE = 1;
								Delay_ms(200);
								DRW_ENABLE = 0;
							}

							mc.mc.rec_start = 0;
							mc.mc.selected = 0;
							selectedAmount = 0;
							selectedQty = 0;
							selectedProd = 0;
							recArrayPtr = 0;
							returnBill = 0;
							if(!mc.mc.udf_en_dis && !mc.mc.kot_en_dis)
							{
								if(mc.mc.rcpt_no >= MAX_RCPTNO)
									mc.mc.rcpt_no = 1;
								else
									mc.mc.rcpt_no++;
							}
							store_mc();
							clrscr();
							double_height = 1;
							double_width = 0;
							gotoxy(3,1);
							lcdprint("     BILL SAVED     ");
							Delay_ms(1000);
							if(mc.mc.gprs_en_dis)
							{
								stopPostCMDToModem();
								pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
							}
							if(mc.mc.gsm_en_dis == RECORD_LOT)
							{
								if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
								{
									sendDayWiseSummary_SMSToAllMobileNumbers();
								}
							}
							udf_no = 0;
						}
						break;
				default:
						break;
			}
		}
	}		//Mrugnil 12/02/24
}
//-------------------------------------------------------------------------
unsigned int productSearchStockView(unsigned int prod, unsigned char enterButtonFlag)
{
	UINT32 loopCount, prodPosition = 0, scrollStart = 0, tempPos = 0;
	UINT8 key;
	BIT exit = 0, shiftFlag = 0, found = 0, scrollFlag = 0,scan_barcode = 1;
	UINT16 prodIndex = 1;

	prodIndex = prod;
	hex_asc(prodIndex,tempArr, 1);
	countSI = 1;
	scrollStart = 1;
	Vikas_Timer = 0;

	while(!exit)
	{
		found = 0;

		if(usb_barcode_enable == TRUE && enterButtonFlag == 1 && scan_barcode == 1)
		{
			if(read_product_barcode() > 0)
			{
				product_code = search_for_duplicate_barcode_details(0);
				FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);
				if(membuf[PRODUCT_EN_FLAG] == ',')
				{
					found = 1;
					prodPosition = product_code;
				}
				else
				{
					found = 0;
					prodPosition = 0;
					prodIndex = 0;
				}
			}
			else
			{
				found = 0;
				prodPosition = 0;
				prodIndex = 0;
			}
		}
		else
		{
			if(scrollFlag)
			{
				prodPosition = scrollStart;
				FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);
				if(membuf[PRODUCT_EN_FLAG] != 1)
					found = 1;
				else
					found = 0;
			}
			else
			{
				prodPosition = productBinarySearch(prodIndex);
//                if(prodPosition > 0)
                if((prodPosition > 0) && (!deletedProduct))		//Mrugnil 27/06/24
                {
                    found = 1;
                    scrollStart = prodPosition;
                }
                else if((prodPosition > 0) && (deletedProduct == 1))		//Mrugnil 27/06/24
                {
                	deletedProduct = 0;		//Mrugnil 27/06/24
                    found = 0;		//Mrugnil 27/06/24
				}
				else
					found = 0;
			}
		}
		scan_barcode = 0;
		if(found)
		{
			FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);

			clrscr();

			double_height = 1;
			double_width = 0;
			gotoxy(1,1);
			lcdprint(" ITEMS STOCK DETAILS");


			double_height = 0;
			double_width = 0;
			gotoxy(3,1);
			for(loopCount = 0; loopCount < PRODUCT_SER_NO_LEN; loopCount++)
			{
				disp_char(membuf[PRODUCT_SER_NO + loopCount]);
			}
//            double_height = 1;		//Mrugnil 10/01/25
//            double_width = 0;
//            gotoxy(4,1);
            //print the product Name
//            for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN - 7; loopCount++)
//            {
//                disp_char(membuf[PRODUCT_TEXT + loopCount]);
//            }		//Mrugnil 10/01/25

			gotoxy(4,1);		//Mrugnil 10/01/25
			for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN; loopCount++)		//Mrugnil 10/01/25
			{
				disp_char(membuf[PRODUCT_TEXT + loopCount]);		//Mrugnil 10/01/25
			}		//Mrugnil 10/01/25

			double_height = 1;
			double_width = 0;
			gotoxy(6,1);
			lcdprint("STOCK: ");

			if(temp_product_stock[prodPosition].temp_product_stock.sign == 1)
				disp_char('-');
			else
				disp_char(' ');

			hex_asc(temp_product_stock[prodPosition].temp_product_stock.totalStock, asc, (PRODUCT_STOCK_LEN - 1));
			rem_zeros(asc, 3);
			for(loopCount = 0; loopCount < (PRODUCT_STOCK_LEN - 1); loopCount++)
			{
				if(loopCount == (PRODUCT_STOCK_LEN - 3))
					disp_char('.');
				disp_char(asc[loopCount]);
			}

			product_code = prodPosition;

			double_width = 0;
			double_height = 0;
			gotoxy(8,1);
			if(!shiftFlag)
				lcdprint("             v/^/CNL");
			else
				lcdprint("SHIFT        v/^/CNL");
		}
		else
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(1,1);
			lcdprint(" ITEMS STOCK DETAILS");

			found = 0;
			double_height = 0;
			double_width = 0;
			gotoxy(3,1);
			hex_asc(prodIndex, asc, 4);
			for(loopCount = 0; loopCount < 4; loopCount++)
				disp_char(asc[loopCount]);

			gotoxy(5,1);
			lcdprint("   ITEM NOT FOUND   ");
			double_width = 0;
			double_height = 0;
			gotoxy(8,1);
			if(!shiftFlag)
				lcdprint("             v/^/CNL");
			else
				lcdprint("SHIFT        v/^/CNL");
		}

		scrollFlag = 0;

		while(!(key = keyPressCheck()));

		switch(key)
		{
		case MENU_KEY:
					if(usb_barcode_enable == TRUE)
						scan_barcode = 1;
					break;
		case DOWN_ARROW:
					if(scrollStart < mc.mc.prod_totalcount)
					{
						tempPos = scrollStart++;
						found = 0;
						while((!found) && (scrollStart <= mc.mc.prod_totalcount))
						{
							FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
							if(membuf[PRODUCT_EN_FLAG] != 1)
								found = 1;
							else
								scrollStart++;

						}
						if(!found)
						{
							scrollStart--;
							while((!found) && (scrollStart > 1))
							{
								FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
								if(membuf[PRODUCT_EN_FLAG] != 1)
									found = 1;
								else
									scrollStart--;
							}
						}
						if(!found)
							scrollStart = tempPos;
						scrollFlag = 1;
					}
					else
					{
						tempPos = scrollStart;
						scrollFlag = 1;
						scrollStart = 1;
						found = 0;
						while((!found) && (scrollStart <= mc.mc.prod_totalcount))
						{
							FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
							if(membuf[PRODUCT_EN_FLAG] != 1)
								found = 1;
							else
								scrollStart++;

						}
						if(!found)
							scrollStart = tempPos;
						scrollFlag = 1;
					}
					break;
		case BACK_SPACE:

						if(shiftFlag)
						{
							if(countSI > 0)
								countSI--;
							if(countSI >= 0)
								tempArr[countSI] = ' ';
							if(countSI <= 0)
							{
								countSI = 0;
								tempArr[countSI] = ' ';

						}
						prodIndex = asc_hex(tempArr, countSI);
						scrollStart = 1;
					}
					else
					{
						if(scrollStart > 1)
						{
							tempPos = scrollStart--;
							found = 0;

							while((!found) && (scrollStart > 0))
							{
								FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
								if(membuf[PRODUCT_EN_FLAG] != 1)
									found = 1;
								else
									scrollStart--;
							}

							if(!found)
							{
								scrollStart++;
								while((!found) && (scrollStart <= mc.mc.prod_totalcount))
								{
									FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
									if(membuf[PRODUCT_EN_FLAG] != 1)
										found = 1;
									else
										scrollStart++;

								}
							}
							if(!found)
								scrollStart = tempPos;
							scrollFlag = 1;
						}
						else
						{
							scrollStart = mc.mc.prod_totalcount;
							scrollFlag = 1;
							found = 0;

							while((!found) && (scrollStart > 0))
							{
								FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
								if(membuf[PRODUCT_EN_FLAG] != 1)
									found = 1;
								else
									scrollStart--;
							}
							if(!found)
								scrollStart = tempPos;
							scrollFlag = 1;
						}
					}
					break;
			case SHIFT_KEY:
						shiftFlag = ~shiftFlag;
						break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
						if((Vikas_Timer > 2000))
						{
							countSI = 0;
							memset(tempArr, '\0', sizeof(tempArr));
							Vikas_Timer = 0;
						}
						if(countSI < 4)
							tempArr[countSI++] = key;
						prodIndex = asc_hex(tempArr, countSI);
						scrollStart = 1;
						break;
			case '*':
						return 0;
			case '#':
						if(enterButtonFlag)
						{
							if((scrollStart <= mc.mc.prod_totalcount) && (scrollStart >= 1) && (found))
								return 1;
						}
						break;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char searchUnit(unsigned char unitCode)
{
	unsigned char loop = 0;
	for(loop = 1; loop <= mc.mc.total_units; loop++)
	{
		FlashReadData((((UINT32) UNIT_TYPE_PG * PGBYTS) + ((UINT32)loop * UNIT_TYPE_LEN)) + UNIT_TYPE_SERIAL_NO, asc, UNIT_TYPE_SERIAL_NO_LEN);
		if(unitCode == asc_hex(asc, UNIT_TYPE_SERIAL_NO_LEN))
		{
			return loop;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
unsigned char enter_manual_rate(unsigned long current_rate)
{
	UINT32 input_rate = 0;

	clrscr();
	double_height = 0;
	gotoxy(7,1);
	lcdprint("QTY : ");
	displayQuantity(qty, unit_fract);
	gotoxy(8,1);
	lcdprint("PRESS       CNL/ENT");
	double_height = 1;
	gotoxy(1,1);
	lcdprint("ENTER RATE :        ");
	recordedDotPosition = 0;
	get_dig_dot_esc_key = 0;
//	input_rate = get_dig_dot(3,6,5,2,current_rate,1);
	input_rate = get_dig_dot(3,6,6,2,current_rate,1);		//Mrugnil 10/04/23

	if(input_rate)
	{
		final_bill_amount = input_rate;
	}
	else if(get_dig_dot_esc_key)
	{
		final_bill_amount = current_rate;
	}
	else
	{
		final_bill_amount = 0;
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned long enterManuaRate(unsigned long current_rate)
{
	UINT32 input_rate = 0;

	clrscr();
	double_height = 0;
	gotoxy(8,1);
	lcdprint("PRESS       CNL/ENT");
	double_height = 1;
	gotoxy(1,1);
	lcdprint("ENTER RATE :        ");
	recordedDotPosition = 0;
	input_rate = get_dig_dot(4,6,6,2,current_rate,1);

	return input_rate;
}
//-------------------------------------------------------------------------
unsigned long get_QUANTITY(unsigned char editFlag)
{
	UINT32 j,ret_qty = 0,tempUTFchar = 0, temp_ret_qty = 0;
	UCHAR i = 0, fract_qty = 0, start_byte = 0, LINE_LEN = 0;

  	clrscr();
	blon();
	double_width  = 0;
	double_height = 0;

	if(!editFlag)
		FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);
	else
		FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)temprec.temprec.product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);
	double_height = 1;
	gotoxy(1,1);

	for(i = PRODUCT_TEXT; i < (PRODUCT_TEXT + 20); i++)
		disp_char(membuf[i]);


	if(!editFlag)
		FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
	else
		FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);

	double_height = 0;
	gotoxy(3, 1);
	lcdprint("ENTER QUANTITY : ");
	double_height = 0;
	if(!mc.mc.udf_en_dis)
	{
		gotoxy(5,1);
		lcdprint("IN ");
		for(i = UNIT_TYPE_NAME; i < (UNIT_TYPE_NAME + UNIT_TYPE_NAME_LEN); i++)
		{
			disp_char(asc[i]);
		}
	}

	if(mc.mc.stock_manage_en_dis)
	{
		gotoxy(7,1);
		lcdprint("STOCK : ");

		if(editFlag)
		{
			if(temp_product_stock[temprec.temprec.product_code].temp_product_stock.sign)
				disp_char('-');
			hex_asc(temp_product_stock[temprec.temprec.product_code].temp_product_stock.totalStock, asc, (PRODUCT_STOCK_LEN - 1));
			rem_zeros(asc, 3);
			for(i = 0; i < (PRODUCT_STOCK_LEN - 1); i++)
			{
				if(i == (PRODUCT_STOCK_LEN - 3))
					disp_char('.');
				disp_char(asc[i]);
			}
		}
		else
		{
			if(temp_product_stock[product_code].temp_product_stock.sign)
				disp_char('-');
			hex_asc(temp_product_stock[product_code].temp_product_stock.totalStock, asc, (PRODUCT_STOCK_LEN - 1));
			rem_zeros(asc, 3);
			for(i = 0; i < (PRODUCT_STOCK_LEN - 1); i++)
			{
				if(i == (PRODUCT_STOCK_LEN - 3))
					disp_char('.');
				disp_char(asc[i]);
			}
		}
	}

	if(editFlag)
		hex_asc(temprec.temprec.rate, asc, 8);
	else
		hex_asc(final_bill_amount, asc, 8);
	rem_zeros(asc, 4);
	gotoxy(8,1);
	lcdprint("RATE : ");
	for(i = 0; i < 8; i++)
	{
		if(i == 6)
			disp_char('.');
		disp_char(asc[i]);
	}
	recordedDotPosition = 0;
	double_height = 1;
	fract_qty = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);
	temprec.temprec.unit_fract = fract_qty;
	unit_fract = fract_qty;
	get_dig_dot_esc_key = 0;
	get_dig_esc_flag = 0;
	if((mc.mc.weighing_scale) && (!editFlag) && (unit_fract))
	{
		ret_qty = readWeighingScaleReading();
		if(ret_qty == 0xffffffff)
		{
			if(mc.mc.qty_enter_method == QTY_3_3)
			{
				if(editFlag)
					ret_qty = temprec.temprec.quantity;
				else
					ret_qty = 1000;
				if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
				{
//					if(temprec.temprec.quantity >= temp_product_stock[product_code].temp_product_stock.totalStock)
					if((ret_qty / 10) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 19/06/25
					{
						if(mc.mc.stock_minus)		//Mrugnil 12/02/24
							ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);		//Mrugnil 12/02/24
						else
						{
							gotoxy(5,13);
							lcdprint("INVALID ENTRY!");		//Mrugnil 12/02/24
							beep(ERRRBEEP);
							Delay_ms(500);
						}
//						ret_qty = temprec.temprec.quantity;		//Mrugnil 30/01/24
//						ret_qty = 0;		//Mrugnil 30/01/24
					}
					else
					{
						ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);
						if((ret_qty / 10) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 19/06/25
						{
							gotoxy(5,13);		//Mrugnil 03/06/25
							lcdprint("INVALID ENTRY!");		//Mrugnil 03/06/25
							beep(ERRRBEEP);		//Mrugnil 03/06/25
							Delay_ms(500);		//Mrugnil 03/06/25
						}		//Mrugnil 03/06/25
					}
				}
				else
					ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);		//Mrugnil 30/01/24
			}
			else
			{
				if(editFlag)
					ret_qty = temprec.temprec.quantity;
				else
					ret_qty = 100;
				if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
				{
//					if(temprec.temprec.quantity >= temp_product_stock[product_code].temp_product_stock.totalStock)
					if(ret_qty > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
					{
						if(mc.mc.stock_minus)		//Mrugnil 12/02/24
							ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);		//Mrugnil 12/02/24
						else
						{
							gotoxy(5,13);
							lcdprint("INVALID ENTRY!");		//Mrugnil 12/02/24
							beep(ERRRBEEP);
							Delay_ms(500);
						}
//						ret_qty = temprec.temprec.quantity;		//Mrugnil 30/01/24
//						ret_qty = 0;		//Mrugnil 30/01/24
					}
					else
					{
						ret_qty = get_dig_dot(5,13,4,2,ret_qty,1);
						if(ret_qty > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
						{
							gotoxy(5,13);		//Mrugnil 03/06/25
							lcdprint("INVALID ENTRY!");		//Mrugnil 03/06/25
							beep(ERRRBEEP);		//Mrugnil 03/06/25
							Delay_ms(500);		//Mrugnil 03/06/25
						}		//Mrugnil 03/06/25
					}
				}
				else
					ret_qty = get_dig_dot(5,13,4,2,ret_qty,1);		//Mrugnil 30/01/24
			}
		}
	}
	else
	{
		switch(fract_qty)
		{
			case 0:
						if(mc.mc.qty_enter_method == QTY_3_3)
						{
							if(editFlag)
								ret_qty = temprec.temprec.quantity / 1000;
							else
								ret_qty = 1;
							get_dig_esc_flag = 0;
							if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
							{
//								if(temprec.temprec.quantity >= temp_product_stock[product_code].temp_product_stock.totalStock)
								if(((ret_qty * 1000) / 10) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
								{
									if(mc.mc.stock_minus)		//Mrugnil 12/02/24
										ret_qty = get_dig(5,13,3,ret_qty,1, 0);		//Mrugnil 12/02/24
									else
									{
										gotoxy(5,13);
										lcdprint("INVALID ENTRY!");		//Mrugnil 12/02/24
										beep(ERRRBEEP);
										Delay_ms(500);
									}
//									ret_qty = temprec.temprec.quantity;		//Mrugnil 30/01/24
//									ret_qty = 0;		//Mrugnil 30/01/24
								}
								else
								{
									ret_qty = get_dig(5,13,3,ret_qty,1, 0);
									if(((ret_qty * 1000) / 10) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 19/06/25
									{
										gotoxy(5,13);		//Mrugnil 03/06/25
										lcdprint("INVALID ENTRY!");		//Mrugnil 03/06/25
										beep(ERRRBEEP);		//Mrugnil 03/06/25
										Delay_ms(500);		//Mrugnil 03/06/25
									}		//Mrugnil 03/06/25
								}
							}
							else
								ret_qty = get_dig(5,13,3,ret_qty,1, 0);		//Mrugnil 30/01/24
							if(ret_qty)
								ret_qty = (ret_qty * 1000);
						}
						else
						{
							if(editFlag)
								ret_qty = temprec.temprec.quantity / 100;
							else
								ret_qty = 1;
							get_dig_esc_flag = 0;
							if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
							{
//								if(temprec.temprec.quantity >= temp_product_stock[product_code].temp_product_stock.totalStock)
								if((ret_qty * 100) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
								{
									if(mc.mc.stock_minus)		//Mrugnil 12/02/24
										ret_qty = get_dig(5,13,4,ret_qty,1, 0);		//Mrugnil 12/02/24
									else
									{
										gotoxy(5,13);
										lcdprint("INVALID ENTRY!");		//Mrugnil 12/02/24
										beep(ERRRBEEP);
										Delay_ms(500);
									}
//									ret_qty = temprec.temprec.quantity;		//Mrugnil 30/01/24
//									ret_qty = 0;		//Mrugnil 30/01/24
								}
								else
								{
									ret_qty = get_dig(5,13,4,ret_qty,1, 0);
									if((ret_qty * 100) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
									{
										gotoxy(5,13);		//Mrugnil 03/06/25
										lcdprint("INVALID ENTRY!");		//Mrugnil 03/06/25
										beep(ERRRBEEP);		//Mrugnil 03/06/25
										Delay_ms(500);		//Mrugnil 03/06/25
									}		//Mrugnil 03/06/25
								}
							}
							else
								ret_qty = get_dig(5,13,4,ret_qty,1, 0);		//Mrugnil 30/01/24
							if(ret_qty)
								ret_qty = (ret_qty * 100);
						}
		   				break;
			case 1:
						if(mc.mc.qty_enter_method == QTY_3_3)
						{
							if(editFlag)
								ret_qty = temprec.temprec.quantity;
							else
								ret_qty = 1000;
							get_dig_dot_esc_key = 0;
							if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
							{
//								if(temprec.temprec.quantity >= temp_product_stock[product_code].temp_product_stock.totalStock)
								if((ret_qty / 10) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 19/06/25
								{
									if(mc.mc.stock_minus)		//Mrugnil 12/02/24
										ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);		//Mrugnil 12/02/24
									else
									{
										gotoxy(5,13);
										lcdprint("INVALID ENTRY!");
										beep(ERRRBEEP);
										Delay_ms(500);
									}
//									ret_qty = temprec.temprec.quantity;		//Mrugnil 30/01/24
//									ret_qty = 0;		//Mrugnil 30/01/24
								}
								else
								{
									ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);
									if((ret_qty / 10) > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 19/06/25
									{
										gotoxy(5,13);		//Mrugnil 03/06/25
										lcdprint("INVALID ENTRY!");		//Mrugnil 03/06/25
										beep(ERRRBEEP);		//Mrugnil 03/06/25
										Delay_ms(500);		//Mrugnil 03/06/25
									}		//Mrugnil 03/06/25
								}
							}
							else
								ret_qty = get_dig_dot(5,13,3,3,ret_qty,1);		//Mrugnil 30/01/24
						}
						else
						{
							if(editFlag)
								ret_qty = temprec.temprec.quantity;
							else
								ret_qty = 100;
							get_dig_dot_esc_key = 0;
							if(mc.mc.stock_manage_en_dis)		//Mrugnil 30/01/24
							{
//								if(temprec.temprec.quantity >= temp_product_stock[product_code].temp_product_stock.totalStock)
								if(ret_qty > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
								{
									if(mc.mc.stock_minus)		//Mrugnil 12/02/24
										ret_qty = get_dig_dot(5,13,4,2,ret_qty,1);		//Mrugnil 12/02/24
									else
									{
										gotoxy(5,13);
										lcdprint("INVALID ENTRY!");		//Mrugnil 12/02/24
										beep(ERRRBEEP);
										Delay_ms(500);
									}
//									ret_qty = temprec.temprec.quantity;		//Mrugnil 30/01/24
//									ret_qty = 0;		//Mrugnil 30/01/24
								}
								else
								{
									ret_qty = get_dig_dot(5,13,4,2,ret_qty,1);
									if(ret_qty > temp_product_stock[product_code].temp_product_stock.totalStock)		//Mrugnil 03/06/25
									{
										gotoxy(5,13);		//Mrugnil 03/06/25
										lcdprint("INVALID ENTRY!");		//Mrugnil 03/06/25
										beep(ERRRBEEP);		//Mrugnil 03/06/25
										Delay_ms(500);		//Mrugnil 03/06/25
									}		//Mrugnil 03/06/25
								}
							}
							else
								ret_qty = get_dig_dot(5,13,4,2,ret_qty,1);		//Mrugnil 30/01/24
						}
		   				break;
		}
	}
	if((mc.mc.stock_manage_en_dis) && (ret_qty))
	{
		if(mc.mc.stock_minus)
		{
			return ret_qty;
		}
		else
		{
        if((temp_product_stock[product_code].temp_product_stock.sign) || (!temp_product_stock[product_code].temp_product_stock.totalStock))
			{
				clrscr();
				double_height = 1;
				double_width = 0;
				gotoxy(3, 1);
				lcdprint(" STOCK IS LESS THAN ");
				gotoxy(5, 1);
				lcdprint("  ENTERED QUANTITY  ");
				beep(ERRRBEEP);
				Delay_ms(1000);
				double_height = 0;
				double_width = 0;
				return 0;
			}
			else
			{
                if(mc.mc.qty_enter_method == QTY_3_3)
                    temp_ret_qty = (ret_qty / 10);
                        else
                    temp_ret_qty = (ret_qty);

                if((temp_ret_qty) <= temp_product_stock[product_code].temp_product_stock.totalStock)
                    return ret_qty;
				else
				{
					clrscr();
					double_height = 1;
					double_width = 0;
					gotoxy(3, 1);
					lcdprint(" STOCK IS LESS THAN ");
					gotoxy(5, 1);
					lcdprint("  ENTERED QUANTITY  ");
					beep(ERRRBEEP);
					Delay_ms(1000);
					double_height = 0;
					double_width = 0;
					return 0;
				}
			}
		}
	}
	else if(ret_qty)
	{
		return ret_qty;
	}
	else
	{
		if(editFlag)
			return temprec.temprec.quantity;
		else
			return 0;
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned long get_BLANK_QUANTITY(unsigned char entered_qty)
{
	UINT32 j = 0, ret_qty = 0, tempUTFchar = 0;
	UCHAR i = 0, fract_qty = 0, start_byte = 0, LINE_LEN = 0;

	ret_qty = entered_qty;

  	clrscr();
	blon();
	double_width  = 0;
	double_height = 0;

	double_height = 0;
	gotoxy(3, 1);
	lcdprint("ENTER QUANTITY : ");
	double_height = 0;

	recordedDotPosition = 0;
	double_height = 1;
	get_dig_dot_esc_key = 0;
	get_dig_esc_flag = 0;
	if(mc.mc.weighing_scale)
	{
		ret_qty = readWeighingScaleReading();
		if(ret_qty == 0xffffffff)
		{
			if(mc.mc.qty_enter_method == QTY_3_3)
			{
//				ret_qty *= 1000;
				ret_qty = get_dig_dot_CUSTOM(5,13,3,3,1,0);
			}
			else
			{
//				ret_qty *= 100;
				ret_qty = get_dig_dot_CUSTOM(5,13,4,2,1,0);
			}
		}
	}
	else
	{
		if(mc.mc.qty_enter_method == QTY_3_3)
		{
//			ret_qty *= 1000;
			get_dig_dot_esc_key = 0;
			ret_qty = get_dig_dot_CUSTOM(5,13,3,3,ret_qty,0);
		}
		else
		{
//			ret_qty *= 100;
			get_dig_dot_esc_key = 0;
			ret_qty = get_dig_dot_CUSTOM(5,13,4,2,ret_qty,0);
		}
	}

	if(ret_qty)
	{
		return ret_qty;
	}
	else
	{
		return 0;
	}
	return 0;
}
//-------------------------------------------------------------------------
void store_entry(unsigned char flagAmount)
{
	UINT16 preTaxDiscRate = 0;
	UINT64 temp_amount = 0, final_discount_amt = 0, tempRate = 0,sgstTemp = 0, cgstTemp = 0;
	UINT32 tem_disc = 0;
	UCHAR temp_arr[PRODUCT_LEN] = {0}, i = 0, j = 0, exit = 0;
	//store date time
	read_rtc();
	rec.rec.strt_byt = 'S';
    rec.rec.dd = tim.tim.dd;
    rec.rec.mm = tim.tim.mm;
    rec.rec.yy = tim.tim.yy;
    rec.rec.hr = tim.tim.hr;
    rec.rec.mn = tim.tim.mn;
    rec.rec.sc = tim.tim.sc;

	rec.rec.bill_stat = 'O';

	rec.rec.product_code = product_code;
	rec.rec.rate = final_bill_amount;
	rec.rec.quantity = qty;
	rec.rec.manualRate = final_bill_amount;
	if(flagAmount)
	{
		rec.rec.final_prod_amount = localAmount;
	}
	else
	{
		rec.rec.final_prod_amount = floatExtractionU64(rec.rec.rate, rec.rec.quantity, FINAL_PRODUCT_AMOUNT);
	}

	rec.rec.unit_type = unit_type;
	rec.rec.unit_fract = unit_fract;
	rec.rec.total_amt = 0;
	rec.rec.agent_no = operatorCode;
	rec.rec.last = 0;
	rec.rec.discount_type = mc.mc.discount_type;
	if(flagIGST)
		rec.rec.igstFlag = BILL_IGST;
	else
		rec.rec.igstFlag = BILL_GST;

	FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)rec.rec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

	removeDot(&membuf[PRODUCT_SGST], asc, PRODUCT_SGST_LEN);
	rec.rec.prd_sgst = asc_hex(asc, strlen(asc));
	rec.rec.sgst_amt = 0;

	removeDot(&membuf[PRODUCT_CGST], asc, PRODUCT_CGST_LEN);
	rec.rec.prd_cgst = asc_hex(asc, strlen(asc));
	rec.rec.cgst_amt = 0;

	if(flagIGST)
	{
		rec.rec.prd_cgst += rec.rec.prd_sgst;
		rec.rec.prd_sgst = 0;
	}

	if(mc.mc.discount_en == ENABLE)
	{
		if(mc.mc.discount_type == ITEMWISE)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)rec.rec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			for(i = 0, j = PRODUCT_DISCOUNT; j < (PRODUCT_DISCOUNT + PRODUCT_DISCOUNT_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);

			if(mc.mc.iw_disc_percent_auto_manual == 1)
			{
//				if(temp_amount)
//				{
//					preTaxDiscRate = temp_amount;
//				}
//				else
//				{
					clrscr();
					double_height = 1;
					gotoxy(1,1);
					lcdprint("ENTER DISCOUNT IN %");
					get_dig_dot_esc_key = 0;
					recordedDotPosition = 0;
					tem_disc = get_dig_dot(4,9,2,2,0,1);
					if(!get_dig_dot_esc_key)
					{
						if(tem_disc)
						{
							preTaxDiscRate = tem_disc;
						}
						else
						{
							preTaxDiscRate = 0;
						}
					}
					else
					{
						preTaxDiscRate = 0;
					}
//				}
			}
			else
			{
				preTaxDiscRate = temp_amount;//vikas
			}

			rec.rec.disc_rate = preTaxDiscRate;//vikas
			rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);
			if(mc.mc.reverse_tax_en_dis)
			{
				if(rec.rec.prd_sgst)
				{
					rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
				}
				if(rec.rec.prd_cgst)
				{
					rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
				}
			}
			else
			{
				if(rec.rec.prd_sgst)
				{
					rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
				}
				else
					rec.rec.sgst_amt = 0;
				if(rec.rec.prd_cgst)
				{
					rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
				}
				else
					rec.rec.cgst_amt = 0;
			}
		}
		else if(mc.mc.discount_type == ITEMWISE_RUPEE)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)rec.rec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			exit = 0;
			while(!exit)
			{
				clrscr();
				double_height = 1;
				gotoxy(1,1);
				lcdprint("ENTER DISCOUNT AMT.");
				hex_asc(rec.rec.final_prod_amount, asc, 9);
				rem_zeros(asc, 5);
				gotoxy(3,1);
				lcdprint("AMT: ");
				for(i = 0; i < 9; i++)
				{
					if(i == 7)
						disp_char('.');
					disp_char(asc[i]);
				}
				get_dig_dot_esc_key = 0;
				recordedDotPosition = 0;
				tem_disc = get_dig_dot(6,9,5,2,0,1);
				if(!get_dig_dot_esc_key)
				{
					if(tem_disc)
					{
						if(tem_disc <= rec.rec.final_prod_amount)
						{
							rec.rec.disc_rate = floatCalculatePercentageU64(rec.rec.final_prod_amount, tem_disc);//vikas
							rec.rec.discount_amt = tem_disc;
							exit = 1;
						}
						else
						{
							clrscr();
							double_height = 1;
							gotoxy(2,1);
							lcdprint(" DISCOUNT EXCEEDING ");
							gotoxy(4,1);
							lcdprint("   PRODUCT AMOUNT   ");
							beep(ERRRBEEP);
							Delay_ms(1000);
						}
					}
					else
					{
						rec.rec.discount_amt = 0;
						exit = 1;
					}
				}
				else
				{
					rec.rec.discount_amt = 0;
					exit = 1;
				}
			}

			if(mc.mc.reverse_tax_en_dis)
			{
				if(rec.rec.prd_sgst)
				{
					rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
				}
				if(rec.rec.prd_cgst)
				{
					rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
				}
			}
			else
			{
				if(rec.rec.prd_sgst)
				{
					rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
				}
				else
				{
					rec.rec.prd_sgst = 0;
					rec.rec.sgst_amt = 0;
				}

				if(rec.rec.prd_cgst)
				{
					rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
				}
				else
				{
					rec.rec.prd_cgst = 0;
					rec.rec.cgst_amt = 0;
				}
			}
		}
		else if(mc.mc.discount_type == PRE_TAX)
		{

		}
		else if(mc.mc.discount_type == PRE_TAX_RUPEE)
		{

		}
		else
		{
			if(mc.mc.reverse_tax_en_dis)
			{
				if(rec.rec.prd_sgst)
				{
					rec.rec.sgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_sgst, MRP);
				}
				if(rec.rec.prd_cgst)
				{
					rec.rec.cgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_cgst, MRP);
				}
			}
			else
			{
				if(rec.rec.prd_sgst)
				{
					rec.rec.sgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_sgst, TAX);
				}
				else
					rec.rec.sgst_amt = 0;

				if(rec.rec.prd_cgst)
				{
					rec.rec.cgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_cgst, TAX);
				}
				else
					rec.rec.cgst_amt = 0;
			}
		}
	}
	else
	{
		rec.rec.discount_amt = 0;

		if(mc.mc.reverse_tax_en_dis)
		{
			if(rec.rec.prd_sgst)
			{
				rec.rec.sgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_sgst, MRP);
			}
			if(rec.rec.prd_cgst)
			{
				rec.rec.cgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_cgst, MRP);
			}
		}
		else
		{
			if(rec.rec.prd_sgst)
			{
				rec.rec.sgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_sgst, TAX);
			}
			else
				rec.rec.sgst_amt = 0;

			if(rec.rec.prd_cgst)
			{
				rec.rec.cgst_amt = floatExtractionU64(rec.rec.final_prod_amount, rec.rec.prd_cgst, TAX);
			}
			else
				rec.rec.cgst_amt = 0;
		}
	}

	selectedQty += rec.rec.quantity;
	selectedAmount += rec.rec.final_prod_amount;
	if(!mc.mc.reverse_tax_en_dis)
	{
		selectedAmount += rec.rec.sgst_amt;
		selectedAmount += rec.rec.cgst_amt;
	}
	if(rec.rec.discount_amt)
		selectedAmount -= rec.rec.discount_amt;
	rec.rec.slab_no = slab_no;

	if(mc.mc.udf_en_dis)
	{
		rec.rec.udf_no = udf_no;
		rec.rec.udf_char = udf_char;
	}
	else
	{
		rec.rec.udf_no = 0;
		rec.rec.udf_char = 0;
	}

	if(mc.mc.return_bill_en_dis)
	{
		if(returnBill)
		{
			rec.rec.calc_stat = 'R';
		}
		else
		{
			rec.rec.calc_stat = 0;
		}
	}
	else
	{
		rec.rec.calc_stat = 0;
	}

	mc.mc.selected++;

	for(i = 0; i < RECDAT_LEN; i++)
	{
		recArray[recArrayPtr][i] = rec.arr[i];
	}
	recArrayPtr++;

	if(!startProd)
	{
		if(mc.mc.currentPG)
			startProd = mc.mc.recs;
		else
			startProd = mc.mc.rec1;
	}
	selectedProd++;
}
//-------------------------------------------------------------------------
//void store_entry_return_bill(void)
//{
//	UINT16 preTaxDiscRate = 0;
//	UINT64 temp_amount = 0, final_discount_amt = 0, tempRate = 0,sgstTemp = 0, cgstTemp = 0;
//	UINT32 tem_disc = 0;
//	UCHAR temp_arr[PRODUCT_LEN] = {0}, i = 0, j = 0, exit = 0;
//	//store date time
//	read_rtc();
//	temprec.temprec.strt_byt = 'S';
//    temprec.temprec.dd = tim.tim.dd;
//    temprec.temprec.mm = tim.tim.mm;
//    temprec.temprec.yy = tim.tim.yy;
//    temprec.temprec.hr = tim.tim.hr;
//    temprec.temprec.mn = tim.tim.mn;
//    temprec.temprec.sc = tim.tim.sc;
//
//	temprec.temprec.bill_stat = 'O';
//
//	temprec.temprec.total_amt = 0;
//	temprec.temprec.agent_no = operatorCode;
//	temprec.temprec.last = 0;
//	temprec.temprec.discount_type = mc.mc.discount_type;
//
//	FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//
//	removeDot(&membuf[PRODUCT_SGST], asc, PRODUCT_SGST_LEN);
//	temprec.temprec.prd_sgst = asc_hex(asc, strlen(asc));
//	temprec.temprec.sgst_amt = 0;
//
//	removeDot(&membuf[PRODUCT_CGST], asc, PRODUCT_CGST_LEN);
//	temprec.temprec.prd_cgst = asc_hex(asc, strlen(asc));
//	temprec.temprec.cgst_amt = 0;
//
//	if(temprec.temprec.igstFlag == BILL_IGST)
//	{
//		temprec.temprec.prd_cgst += temprec.temprec.prd_sgst;
//		temprec.temprec.prd_sgst = 0;
//	}
//
//	if(mc.mc.discount_en == ENABLE)
//	{
//		if(mc.mc.discount_type == ITEMWISE)
//		{
//			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//
//			for(i = 0, j = PRODUCT_DISCOUNT; j < (PRODUCT_DISCOUNT + PRODUCT_DISCOUNT_LEN); j++)
//			{
//				if(membuf[j] != '.')
//					temp_arr[i++] = membuf[j];
//			}
//			temp_amount = 0;
//			temp_amount = asc_hex(temp_arr, i);
//
//			if(mc.mc.iw_disc_percent_auto_manual == 1)
//			{
////				if(temp_amount)
////				{
////					preTaxDiscRate = temp_amount;
////				}
////				else
////				{
//					clrscr();
//					double_height = 1;
//					gotoxy(1,1);
//					lcdprint("ENTER DISCOUNT IN %");
//					get_dig_dot_esc_key = 0;
//					recordedDotPosition = 0;
//					tem_disc = get_dig_dot(4,9,2,2,0,1);
//					if(!get_dig_dot_esc_key)
//					{
//						if(tem_disc)
//						{
//							preTaxDiscRate = tem_disc;
//						}
//						else
//						{
//							preTaxDiscRate = 0;
//						}
//					}
//					else
//					{
//						preTaxDiscRate = 0;
//					}
////				}
//			}
//			else
//			{
//				preTaxDiscRate = temp_amount;//vikas
//			}
//			temprec.temprec.disc_rate = preTaxDiscRate;//vikas
//			temprec.temprec.discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount , preTaxDiscRate, DISCOUNT);
//
//			if(mc.mc.reverse_tax_en_dis)
//			{
//				if(temprec.temprec.prd_sgst)
//				{
//					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, MRP);
//				}
//				if(temprec.temprec.prd_cgst)
//				{
//					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, MRP);
//				}
//			}
//			else
//			{
//				if(temprec.temprec.prd_sgst)
//					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, TAX);
//				else
//					temprec.temprec.sgst_amt = 0;
//
//				if(temprec.temprec.prd_cgst)
//					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, TAX);
//				else
//					temprec.temprec.cgst_amt = 0;
//			}
//		}
//		else if(mc.mc.discount_type == ITEMWISE_RUPEE)
//		{
//			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//
//			exit = 0;
//			while(!exit)
//			{
//				clrscr();
//				double_height = 1;
//				gotoxy(1,1);
//				lcdprint("ENTER DISCOUNT AMT.");
//				hex_asc(temprec.temprec.final_prod_amount, asc, 9);
//				rem_zeros(asc, 5);
//				gotoxy(3,1);
//				lcdprint("AMT: ");
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						disp_char('.');
//					disp_char(asc[i]);
//				}
//				get_dig_dot_esc_key = 0;
//				recordedDotPosition = 0;
//				tem_disc = get_dig_dot(6,9,5,2,0,1);
//				if(!get_dig_dot_esc_key)
//				{
//					if(tem_disc)
//					{
//						if(tem_disc <= temprec.temprec.final_prod_amount)
//						{
//							temprec.temprec.disc_rate = floatCalculatePercentageU64(temprec.temprec.final_prod_amount, tem_disc);//vikas
//							temprec.temprec.discount_amt = tem_disc;
//							exit = 1;
//						}
//						else
//						{
//							clrscr();
//							double_height = 1;
//							gotoxy(2,1);
//							lcdprint(" DISCOUNT EXCEEDING ");
//							gotoxy(4,1);
//							lcdprint("   PRODUCT AMOUNT   ");
//							beep(ERRRBEEP);
//							Delay_ms(1000);
//						}
//					}
//					else
//					{
//						temprec.temprec.discount_amt = 0;
//						exit = 1;
//					}
//				}
//				else
//				{
//					temprec.temprec.discount_amt = 0;
//					exit = 1;
//				}
//			}
//
//			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
//			{
//				if(membuf[j] != '.')
//					temp_arr[i++] = membuf[j];
//			}
//			temp_amount = 0;
//			temp_amount = asc_hex(temp_arr, i);
//			temprec.temprec.prd_sgst = temp_amount;
//
//			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
//			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
//			{
//				if(membuf[j] != '.')
//					temp_arr[i++] = membuf[j];
//			}
//			temp_amount = 0;
//			temp_amount = asc_hex(temp_arr, i);
//			temprec.temprec.prd_cgst = temp_amount;
//
//			if(mc.mc.reverse_tax_en_dis)
//			{
//				if(temprec.temprec.prd_sgst)
//				{
//					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, MRP);
//				}
//				if(temprec.temprec.prd_cgst)
//				{
//					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, MRP);
//				}
//				else
//				{
//					if(temprec.temprec.prd_sgst)
//						temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, TAX);
//					else
//						temprec.temprec.sgst_amt = 0;
//
//					if(temprec.temprec.prd_cgst)
//						temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, TAX);
//					else
//						temprec.temprec.cgst_amt = 0;
//				}
//			}
//		}
//		else if(mc.mc.discount_type == PRE_TAX)
//		{
//
//		}
//		else if(mc.mc.discount_type == PRE_TAX_RUPEE)
//		{
//
//		}
//		else
//		{
//			if(mc.mc.reverse_tax_en_dis)
//			{
//				if(temprec.temprec.prd_sgst)
//				{
//					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, MRP);
//				}
//				if(temprec.temprec.prd_cgst)
//				{
//					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, MRP);
//				}
//			}
//			else
//			{
//				if(temprec.temprec.prd_sgst)
//					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, TAX);
//				else
//					temprec.temprec.sgst_amt = 0;
//
//				if(temprec.temprec.prd_cgst)
//					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, TAX);
//				else
//					temprec.temprec.cgst_amt = 0;
//			}
//		}
//	}
//	else
//	{
//		temprec.temprec.discount_amt = 0;
//
//		if(mc.mc.reverse_tax_en_dis)
//		{
//			if(temprec.temprec.prd_sgst)
//			{
//				temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, MRP);
//			}
//			if(temprec.temprec.prd_cgst)
//			{
//				temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, MRP);
//			}
//		}
//		else
//		{
//			if(temprec.temprec.prd_sgst)
//				temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, TAX);
//			else
//				temprec.temprec.sgst_amt = 0;
//
//			if(temprec.temprec.prd_cgst)
//				temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, TAX);
//			else
//				temprec.temprec.cgst_amt = 0;
//		}
//	}
//
//	selectedQty += temprec.temprec.quantity;
//	selectedAmount += temprec.temprec.final_prod_amount;
//	if(!mc.mc.reverse_tax_en_dis)
//	{
//		selectedAmount += temprec.temprec.sgst_amt;
//		selectedAmount += temprec.temprec.cgst_amt;
//	}
//	if(temprec.temprec.discount_amt)
//		selectedAmount -= temprec.temprec.discount_amt;
//	temprec.temprec.slab_no = slab_no;
//
//	if(mc.mc.udf_en_dis)
//	{
//		temprec.temprec.udf_no = udf_no;
//		temprec.temprec.udf_char = udf_char;
//	}
//	else
//	{
//		temprec.temprec.udf_no = 0;
//		temprec.temprec.udf_char = 0;
//	}
//	if(mc.mc.return_bill_en_dis)
//	{
//		if(temprec.temprec.returnedProduct == 1)
//		{
//			temprec.temprec.calc_stat = 'R';
//		}
//		else
//		{
//			temprec.temprec.calc_stat = 0;
//		}
//	}
//	else
//	{
//		temprec.temprec.calc_stat = 0;
//	}
//}
//-------------------------------------------------------------------------
unsigned long long calculate_rate(void)
{
	UINT64 item_rate = 0;
	UINT64 temp_amount = 0;

	temp_amount = floatExtractionU64(final_bill_amount, qty, FINAL_PRODUCT_AMOUNT);
	return (temp_amount);
}
//-------------------------------------------------------------------------
unsigned long long mult64_32_32(volatile unsigned long a, volatile unsigned long b)
{
    unsigned long long result;

    result = floatExtractionU64(b, a, FINAL_PRODUCT_AMOUNT);
	return result;
}
//-------------------------------------------------------------------------
//unsigned char searchProductByName_test(void)
unsigned char searchProductByName_test(unsigned char usedInEditingFunction)		//Mrugnil 27/06/24
{
	unsigned char exitMain = 0, key = 0, printFlag = 0;
	BIT idfound = 0, h, w;
	unsigned char row, shiftFlag = 0;
	UCHAR identer = 0;
	char custID_arr[PRODUCT_TEXT_LEN + 1] = {'0'}, custID_cmp_arr[PRODUCT_TEXT_LEN + 1];
	UINT32 prodcount = 1, j = 0, scroll_count = 0, loopUsercount = 1, loopUserdis = 0, i = 0;
	UINT16 loopCount = 0, storeCount = 0, endpositionProduct = 0;
	char *tempArry;

	prodcount = mc.mc.prod_totalcount;
	move_frwd_flag = 1;
	doubleKeyPressCheck = 0;
	loopUsercount = 0;
	idfound = 1;
	deletedProduct = 0;		//Mrugnil 27/06/24
	row = 3;
	identer = 0;
	memset(custID_arr, 0, PRODUCT_TEXT_LEN + 1);
	memset(custID_cmp_arr, 0, PRODUCT_TEXT_LEN + 1);

	endpositionProduct = prodcount;

	for(i = 1, loopUserdis = 0;(i < prodcount); i++)
	{
		FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) i * PRODUCT_LEN)), asc, PRODUCT_LEN);
		if(asc[PRODUCT_EN_FLAG] != 1)
		{
			g_ID_buf[loopUserdis] = i;
			loopUserdis++;
			endpositionProduct = loopUserdis;
			idfound = 1;
			deletedProduct = 0;		//Mrugnil 27/06/24
		}
		if((asc[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction))		//Mrugnil 27/06/24
		{
			g_ID_buf[loopUserdis] = i;
			loopUserdis++;
			endpositionProduct = loopUserdis;
			idfound = 1;
			deletedProduct = 1;
		}
		loopUsercount = 0;
	}
	if(!loopUserdis)
		idfound = 0;
	loopUserdis = 0;

	while(!exitMain)
	{
		clrscr();
		gotoxy(1,1);
		double_width = 0;
		double_height = 1;
		lcdprint("ENTER PROD NAME : ");
		double_width = 0;
		double_height = 0;
		gotoxy(8,1);
		lcdprint("        ENT/CNL/^/v");
		row = 3;
		clrRow(row);
		if(blink_flag)
			disp_char('_');

		gotoxy(row,1);
		for(loopUserdis = 0; loopUserdis < identer; loopUserdis++)
		{
			disp_char(custID_arr[loopUserdis]);
		}

		if((g_ID_buf[loopUsercount]<=prodcount) && (g_ID_buf[loopUsercount] > 0) && (endpositionProduct >= loopUsercount))
		{
			FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)g_ID_buf[loopUsercount] * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
			double_width = 0;
			double_height = 1;
			gotoxy(++row,1);

			if(!deletedProduct)		//Mrugnil 27/06/24
			{
				for(loopUserdis = 0; loopUserdis < (PRODUCT_TEXT_LEN - 7); loopUserdis++)
				{
					disp_char(custID_cmp_arr[loopUserdis]);
				}
			}
			else if(deletedProduct == 1)		//Mrugnil 27/06/24
			{
				lcdprint("   DELETED PRODUCT  ");		//Mrugnil 27/06/24
			}
			product_code = g_ID_buf[loopUsercount];
			scroll_count = loopUsercount;
			loopUsercount++;
			double_width = 0;
			double_height = 0;
			row++;
		}
		else
		{
			double_width = 0;
			double_height = 1;
			gotoxy(++row,1);
			lcdprint(" PRODUCT NOT FOUND! ");
			double_width = 0;
			double_height = 0;
			row++;
		}

		if((g_ID_buf[loopUsercount]<=prodcount) && (g_ID_buf[loopUsercount] > 0) && (endpositionProduct >= loopUsercount))
		{
			gotoxy(++row,1);
			FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)g_ID_buf[loopUsercount] * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
			if(!deletedProduct)		//Mrugnil 27/06/24
			{
				for(loopUserdis = 0; loopUserdis < (PRODUCT_TEXT_LEN - 7); loopUserdis++)
					disp_char(custID_cmp_arr[loopUserdis]);
			}
			else if(deletedProduct == 1)		//Mrugnil 27/06/24
			{
				lcdprint("   DELETED PRODUCT  ");		//Mrugnil 27/06/24
			}
			loopUsercount++;
		}
		else
		{
			gotoxy(++row,1);
			lcdprint("                    ");
			loopUsercount++;
		}

		if((g_ID_buf[loopUsercount]<=prodcount) && (g_ID_buf[loopUsercount] > 0) && (endpositionProduct >= loopUsercount))
		{
			gotoxy(++row,1);
			FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)g_ID_buf[loopUsercount] * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
			if(!deletedProduct)		//Mrugnil 27/06/24
			{
				for(loopUserdis = 0; loopUserdis < (PRODUCT_TEXT_LEN - 7); loopUserdis++)
					disp_char(custID_cmp_arr[loopUserdis]);
			}
			else if(deletedProduct == 1)		//Mrugnil 27/06/24
			{
				lcdprint("   DELETED PRODUCT  ");		//Mrugnil 27/06/24
			}
		}
		else
		{
			gotoxy(++row,1);
			lcdprint("                    ");
		}

		if(shiftFlag)
		{
			h = double_height;
			w = double_width;
			double_height = 0;
			double_width = 0;
			gotoxy(8,1);
			lcdprint("SHIFT");
			double_height = h;
			double_width = w;
		}
		else
		{
			h = double_height;
			w = double_width;
			double_height = 0;
			double_width = 0;
			gotoxy(8,1);
			lcdprint("    ");
			double_height = h;
			double_width = w;
		}

		deletedProduct = 0;		//Mrugnil 27/06/24
		while(!(key = keyPressCheck()));
		switch(key)
		{
			case UP_ARROW:
						if(shiftFlag)
						{
							if(identer > 0)
							{
								identer--;
								custID_arr[identer] = 0;
								for(j = 0; j < MAX_PRODUCTS; j++)
									g_ID_buf[j] = 0;

								idfound = 0;

								deletedProduct = 0;		//Mrugnil 27/06/24

								for(i = 1, loopUserdis = 0;(i <= prodcount); i++)
								{
									deletedProduct = 0;		//Mrugnil 27/06/24
									FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) i * PRODUCT_LEN)), asc, PRODUCT_LEN);
									if(asc[PRODUCT_EN_FLAG] != 1)
									{
										FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)i * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
										if(custID_arr[0] == custID_cmp_arr[0])
										{
											if(custID_arr[1] != ' ')
											{
												if(strncmp((const char *)custID_arr, (const char *)custID_cmp_arr, identer) == 0)
												{
													g_ID_buf[loopUserdis] = i;
													loopUserdis++;
													endpositionProduct = loopUserdis;
													idfound = 1;
													deletedProduct = 0;		//Mrugnil 27/06/24
												}
											}
											else
											{
												tempArry = &custID_arr[1];
												if(strstr(custID_cmp_arr, tempArry) != NULL)
												{
													g_ID_buf[loopUserdis] = i;
													loopUserdis++;
													endpositionProduct = loopUserdis;
													idfound = 1;
													deletedProduct = 0;		//Mrugnil 27/06/24
												}
											}
										}
									}
									else if((asc[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction))		//Mrugnil 27/06/24
									{
										FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)i * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
										if(custID_arr[0] == custID_cmp_arr[0])
										{
											if(custID_arr[1] != ' ')
											{
												if(strncmp((const char *)custID_arr, (const char *)custID_cmp_arr, identer) == 0)
												{
													g_ID_buf[loopUserdis] = i;
													loopUserdis++;
													endpositionProduct = loopUserdis;
													idfound = 1;
													deletedProduct = 1;		//Mrugnil 27/06/24
												}
											}
											else
											{
												tempArry = &custID_arr[1];
												if(strstr(custID_cmp_arr, tempArry) != NULL)
												{
													g_ID_buf[loopUserdis] = i;
													loopUserdis++;
													endpositionProduct = loopUserdis;
													idfound = 1;
													deletedProduct = 1;		//Mrugnil 27/06/24
												}
											}
										}
									}
									loopUsercount = 0;
								}
								if(identer == 0)
								{
									for(j = 1; j <= prodcount; j++)
										g_ID_buf[j-1] = j;

									endpositionProduct = prodcount;
									loopUsercount = 0;
									idfound = 1;
								}
							}
							else
							{
								for(j = 1; j <= prodcount; j++)
									g_ID_buf[j-1] = j;

								endpositionProduct = prodcount;
								loopUsercount = 0;
							}
						}
						else
						{
							if(loopUsercount > 0x02)
							{
								row = 3;
								loopUsercount = (loopUsercount - 3);
							}
							else
							{
								loopUsercount = 0;
								row = 3;
							}
						}
						break;
			case DOWN_ARROW:
						if(scroll_count< (endpositionProduct - 1))
						{
							row = 3;
							loopUsercount = (scroll_count + 1);
						}
						else
						{
							loopUsercount = 0;
							row = 3;
						}
						break;
			case SHIFT_KEY:
						shiftFlag = ~shiftFlag;
						if(shiftFlag)
						{
							h = double_height;
							w = double_width;
							double_height = 0;
							double_width = 0;
							gotoxy(8,1);
							lcdprint("SHIFT");
							double_height = h;
							double_width = w;
						}
						else
						{
							h = double_height;
							w = double_width;
							double_height = 0;
							double_width = 0;
							gotoxy(8,1);
							lcdprint("     ");
							double_height = h;
							double_width = w;
						}
						loopUsercount = scroll_count;
						break;
			case '#':
						if(idfound)
						{
//							product_code = scroll_count;
							exitMain = 1;
						}
						else
							loopUsercount = scroll_count;
						break;
			case PRINT_KEY:
						if(mc.mc.product_selection == BEFORE_QTY)
						{
							if(!recArrayPtr && !returnBill && searchProductSingle)
							{
								if((idfound) && (!deletedProduct))		//Mrugnil 27/06/24
								{
									exitMain = 1;
									printFlag = 1;
								}
								else
									loopUsercount = scroll_count;
							}
						}
						break;
			case MENU_KEY:
//						if(!recArrayPtr && !returnBill)
//						{
//							if(idfound)
//							{
//								exitMain = 1;
//								printFlag = 2;
//							}
//							else
//								loopUsercount = scroll_count;
//						}
						if(!mc.mc.udf_en_dis && !returnBill)
						{
							if(idfound)
							{
								exitMain = 1;
								if(!deletedProduct)		//Mrugnil 27/06/24
									printFlag = 2;
							}
							else
								loopUsercount = scroll_count;
						}
						break;
			case '*':
						return 0;
			default:
						if((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z') || (key == 0x20))
						{
							if(identer < (PRODUCT_TEXT_LEN - 7))
							{
								if((doubleKeyPressCheckWait > 0) && (!move_frwd_flag))
								{
									custID_arr[identer - 1] = key;
									doubleKeyPressCheckWait = 0;
								}
								else if((shiftFlag) && (key == '0'))
								{
									custID_arr[identer++] = ' ';
									doubleKeyPressCheckWait = 0;
								}
								else
									custID_arr[identer++] = key;
							}
							clrRow(3);
							gotoxy(3,1);
							double_width = 0;
							double_height = 0;
							for(i=0;i<identer;i++)
								disp_char(custID_arr[i]);
							loopUsercount = 0;

							for(j = 0; j < MAX_PRODUCTS; j++)
								g_ID_buf[j] = 0;

							idfound = 0;

							deletedProduct = 0;		//Mrugnil 27/06/24

							for(i = 1, loopUserdis = 0;(i <= prodcount); i++)
							{
								deletedProduct = 0;		//Mrugnil 27/06/24
								FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) i * PRODUCT_LEN)), asc, PRODUCT_LEN);
								if(asc[PRODUCT_EN_FLAG] != 1)
								{
									FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)i * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
									if(custID_arr[0] == custID_cmp_arr[0])
									{
										if(custID_arr[1] != ' ')
										{
											if(strncmp((const char *)custID_arr, (const char *)custID_cmp_arr, identer) == 0)
											{
												g_ID_buf[loopUserdis] = i;
												loopUserdis++;
												endpositionProduct = loopUserdis;
												idfound = 1;
												deletedProduct = 0;		//Mrugnil 27/06/24
											}
										}
										else
										{
											tempArry = &custID_arr[1];
											if(strstr(custID_cmp_arr, tempArry))
											{
												g_ID_buf[loopUserdis] = i;
												loopUserdis++;
												endpositionProduct = loopUserdis;
												idfound = 1;
												deletedProduct = 0;		//Mrugnil 27/06/24
											}
										}
									}
								}
								else if((asc[PRODUCT_EN_FLAG] == 1) && (usedInEditingFunction))		//Mrugnil 27/06/24
								{
									FlashReadData((unsigned long)((unsigned long)(PRODUCT_PG * PGBYTS) + (unsigned long)i * PRODUCT_LEN + PRODUCT_TEXT), custID_cmp_arr, PRODUCT_TEXT_LEN);
									if(custID_arr[0] == custID_cmp_arr[0])
									{
										if(custID_arr[1] != ' ')
										{
											if(strncmp((const char *)custID_arr, (const char *)custID_cmp_arr, identer) == 0)
											{
												g_ID_buf[loopUserdis] = i;
												loopUserdis++;
												endpositionProduct = loopUserdis;
												idfound = 1;
												deletedProduct = 1;		//Mrugnil 20/06/24
											}
										}
										else
										{
											tempArry = &custID_arr[1];
											if(strstr(custID_cmp_arr, tempArry))
											{
												g_ID_buf[loopUserdis] = i;
												loopUserdis++;
												endpositionProduct = loopUserdis;
												idfound = 1;
												deletedProduct = 1;		//Mrugnil 20/06/24
											}
										}
									}
								}
							}
							loopUsercount = 0;
						}
						else
							loopUsercount = scroll_count;
						break;
		}
	}

	if(idfound == 1)
	{
		FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);

		switch(slab_no)
		{
			case 1:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT_LEN - 1);
					break;
			case 2:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT2_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT2 + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT2 + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT2_LEN - 1);
					break;
			case 0:
			case 3:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT3_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT3 + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT3 + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT3_LEN - 1);
					break;

		}

		//get product discount
		for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_DISCOUNT_LEN; loopCount++)
		{
			if(membuf[PRODUCT_DISCOUNT + loopCount] != '.')
			{
				asc[storeCount++] = membuf[PRODUCT_DISCOUNT + loopCount];
			}
		}
		prod_discount = asc_hex(asc, PRODUCT_DISCOUNT_LEN - 1);

		//get product SGST
		for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_SGST_LEN; loopCount++)
		{
			if(membuf[PRODUCT_SGST + loopCount] != '.')
			{
				asc[storeCount++] = membuf[PRODUCT_SGST + loopCount];
			}
		}
		prod_vat = asc_hex(asc, PRODUCT_SGST_LEN - 1);

		unit_type = asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN);

		FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
		unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);

//		if(printFlag == 1)
		if((printFlag == 1) && (!deletedProduct))		//Mrugnil 27/06/24
		{
			oneProductSale(product_code);
			return 2;
		}
		else if((printFlag == 2) && (!deletedProduct))		//Mrugnil 27/06/24
		{
			return 3;
		}
		else
			return 1;
	}
	else
		return 0;
}
//-------------------------------------------------------------------------
void clrRow(unsigned char row)
{
	unsigned char i;
	gotoxy(row,1);
	for(i=0;i<20;i++)
		disp_char(' ');
}
//-------------------------------------------------------------------------
unsigned char checkForLastRecordSaved(void)
{
	unsigned long loop = 0, rcptNo = 0;
	unsigned char found = 0;
	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);
		startRec = udfno.udfno.udfStart;
		if(mc.mc.currentPG)
			endRec = mc.mc.recs;
		else
			endRec = mc.mc.rec1;

		rcptNo = udfno.udfno.udfRcptNo;
	}
	else
	{
		startRec = mc.mc.rec_start;
		if(mc.mc.currentPG)
			endRec = mc.mc.recs;
		else
			endRec = mc.mc.rec1;

		get_rec(startRec);
		rcptNo = rec.rec.rcpt_no;
	}
	for(loop = startRec; loop < endRec; loop++)
	{
		get_rec(loop);

		if((rec.rec.last == 1) && (rec.rec.rcpt_no == rcptNo) && (rec.rec.bill_stat == 'O'))
			found = 1;
	}

	if(found)
		return 1;
	else
		return 0;
}
//-------------------------------------------------------------------------
unsigned char checkForLastReturnRecordSaved(unsigned char billType)
{
	unsigned long loop = 0, rcptNo = 0;
	unsigned char found = 0;
	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);
		startRec = udfno.udfno.udfStart;
		if(mc.mc.currentPG)
			endRec = mc.mc.recs;
		else
			endRec = mc.mc.rec1;
	}
	else
	{
		if(!billType)
			startRec = mc.mc.rec_start;
		if(mc.mc.currentPG)
			endRec = mc.mc.recs;
		else
			endRec = mc.mc.rec1;
	}
	if(billType)
	{
		rcptNo = mc.mc.rcpt_no;
	}
	else
	{
		rcptNo = mc.mc.rcpt_no;
	}
	for(loop = startRec; loop < endRec; loop++)
	{
		get_rec(loop);

		if((rec.rec.last == 1) && (rec.rec.rcpt_no == rcptNo) && (rec.rec.bill_stat == 'O'))
			found = 1;
	}

	if(found)
		return 1;
	else
		return 0;
}
//-------------------------------------------------------------------------
unsigned char store_last_rec(void)
{
	UINT32 loop = 0, preTaxDiscRate = 0, preTaxDiscRupee = 0;
	UINT64 final_amt = 0, final_discount_amt = 0;
	UINT64 final_prod_amount = 0, final_tot_amount = 0, service_tax = 0, vat_amount = 0, temp_amount = 0, service_amount = 0;
	UCHAR i = 0, j = 0, tempSlab = 0, exit = 0, found = 0, temp_arr[PRODUCT_LEN] = {0};
	UINT32 loopProdSumm = 0, SUMMARY_START_ADD = 0, final_quantity = 0, temp_parcel_amt = 0; //vikas
	UINT16 cust_det = 0, temp_parcel_rate = 0; //vikas

	if(checkForLastRecordSaved())
		return 1;

	//store date time
	read_rtc();
	rec.rec.total_amt = 0;

	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);
		startRec = udfno.udfno.udfStart;
		if(mc.mc.currentPG)
			endRec = mc.mc.recs;
		else
			endRec = mc.mc.rec1;
	}
	else
	{
		startRec = mc.mc.rec_start;
		if(mc.mc.currentPG)
			endRec = mc.mc.recs;
		else
			endRec = mc.mc.rec1;
	}

	final_prod_amount = 0;
	vat_amount = 0;
	service_amount = 0;
	sgstArrayPtr = 0;
	cgstArrayPtr = 0;
	recArrayPtr = 0;
	g_prodPosArrayPtr = 0;
	g_prodArrayPtr = 0;
	final_discount_amt = 0;
	final_quantity = 0;
	for(loop = startRec, i = 0; loop < endRec; loop++)
	{
		get_rec(loop);

		if(mc.mc.udf_en_dis)
		{
			if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O'))
			{
				final_amt += rec.rec.final_prod_amount;
				tempSlab = rec.rec.slab_no;
				final_quantity++;
				vat_amount += rec.rec.sgst_amt;
				sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
				service_amount += rec.rec.cgst_amt;
				cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;

				final_discount_amt += rec.rec.discount_amt;
				for(i = 0; i < RECDAT_LEN; i++)
					recArray[recArrayPtr][i] = rec.arr[i];
				recArrayPtr++;
				g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
				prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
				prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
			}
		}
		else
		{
			if((rec.rec.bill_stat == 'O'))
			{
				final_amt += rec.rec.final_prod_amount;
				final_quantity++;
				vat_amount += rec.rec.sgst_amt;
				sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
				service_amount += rec.rec.cgst_amt;
				cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;

				final_discount_amt += rec.rec.discount_amt;
				for(i = 0; i < RECDAT_LEN; i++)
					recArray[recArrayPtr][i] = rec.arr[i];
				recArrayPtr++;
				g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
				prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
				prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
			}
		}
	}

	sortVATAndServiceArray();		//Mrugnil 31/03/25

	if(final_amt)
		final_prod_amount = final_amt;
	else
		final_prod_amount = 0;

	final_tot_amount = final_prod_amount;

	final_amt = 0;
	if(mc.mc.discount_en == ENABLE)
	{
		switch(mc.mc.discount_type)
		{
			case		ITEMWISE_RUPEE:
									final_amt = 0;
									for(loop = startRec; loop < endRec; loop++)
									{
										get_rec(loop);

										if(mc.mc.udf_en_dis)
										{
											if((rec.rec.discount_amt) && (rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O'))
												final_amt += rec.rec.discount_amt;
										}
										else
										{
											if((rec.rec.discount_amt) && (rec.rec.bill_stat == 'O'))
												final_amt += rec.rec.discount_amt;
										}
									}
									final_discount_amt = final_amt;
									final_tot_amount = (final_prod_amount - final_discount_amt);
									if(!mc.mc.reverse_tax_en_dis)
									{
										final_tot_amount += vat_amount;
										final_tot_amount += service_amount;
									}
									if(config.config.other_charges == 1)
									{
										temp_amount = config.config.other_chrg_rate;
										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
										final_tot_amount = (final_tot_amount + service_tax);
									}

									break;
			case 	     ITEMWISE:
									final_tot_amount = final_prod_amount;

									final_tot_amount -= final_discount_amt;
									if(!mc.mc.reverse_tax_en_dis)
									{
										final_tot_amount += vat_amount;
										final_tot_amount += service_amount;
									}
									if(config.config.other_charges == 1)
									{
										temp_amount = config.config.other_chrg_rate;
										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
										final_tot_amount = (final_tot_amount + service_tax);
									}
									break;
			case          PRE_TAX:
									if(mc.mc.udf_en_dis)
									{
										clrscr();
										double_height = 1;
										gotoxy(1,1);
										lcdprint("ENTER DISCOUNT IN %");
										get_dig_dot_esc_key = 0;
										recordedDotPosition = 0;
										final_amt = get_dig_dot(4,9,2,2,0,1);
										if(!get_dig_dot_esc_key)
										{
											if(final_amt)
											{
												preTaxDiscRate = final_amt;
											}
											else
											{
												final_discount_amt = 0;
												preTaxDiscRate = 0;
											}
										}
										else
										{
											final_discount_amt = 0;
											preTaxDiscRate = 0;
										}

										vat_amount = 0;
										service_amount = 0;
										final_discount_amt = 0;

										managePreTaxDiscountFromRAMAndStore(startRec, preTaxDiscRate);

										final_prod_amount = 0;
										vat_amount = 0;
										service_amount = 0;
										sgstArrayPtr = 0;
										cgstArrayPtr = 0;
										recArrayPtr = 0;
										g_prodPosArrayPtr = 0;
										g_prodArrayPtr = 0;
										final_discount_amt = 0;
										for(loop = startRec, i = 0; loop < endRec; loop++)
										{
											get_rec(loop);

											if(mc.mc.udf_en_dis)
											{
												if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O'))
												{
													final_prod_amount += rec.rec.final_prod_amount;
													tempSlab = rec.rec.slab_no;
													if(rec.rec.sgst_amt)
													{
														vat_amount += rec.rec.sgst_amt;
														sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;

													}
													if(rec.rec.cgst_amt)
													{
														service_amount += rec.rec.cgst_amt;
														cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
													}
													for(i = 0; i < RECDAT_LEN; i++)
														recArray[recArrayPtr][i] = rec.arr[i];
													final_discount_amt += rec.rec.discount_amt;
													recArrayPtr++;
													g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
													prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
													prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
												}
											}
											else
											{
												if((rec.rec.bill_stat == 'O'))
												{
													final_prod_amount += rec.rec.final_prod_amount;
													if(rec.rec.sgst_amt)
													{
														vat_amount += rec.rec.sgst_amt;
														sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;

													}
													if(rec.rec.cgst_amt)
													{
														service_amount += rec.rec.cgst_amt;
														cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
													}
													for(i = 0; i < RECDAT_LEN; i++)
														recArray[recArrayPtr][i] = rec.arr[i];
													final_discount_amt += rec.rec.discount_amt;
													recArrayPtr++;
													g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
													prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
													prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
												}
											}
										}
									}

									sortVATAndServiceArray();		//Mrugnil 31/03/25

									final_tot_amount = final_prod_amount;

									final_tot_amount -= final_discount_amt;
									if(!mc.mc.reverse_tax_en_dis)
									{
										final_tot_amount += vat_amount;
										final_tot_amount += service_amount;
									}
									if(config.config.other_charges == 1)
									{
										temp_amount = config.config.other_chrg_rate;
										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
										final_tot_amount = (final_tot_amount + service_tax);
									}
									break;
			case	PRE_TAX_RUPEE:
									if(mc.mc.udf_en_dis)
									{
										while(!exit)
										{
											final_amt = 0;
											pretax_discount_rupee = 0;
											clrscr();
											double_height = 1;
											gotoxy(1,1);
											lcdprint("ENTER DISCOUNT IN Rs");
											get_dig_dot_esc_key = 0;
											recordedDotPosition = 0;
											preTaxDiscRupee = get_dig_dot(4,9,5,2,0,1);
											if(!get_dig_dot_esc_key)
											{
												if(preTaxDiscRupee < final_prod_amount)
												{
													pretax_discount_rupee = preTaxDiscRupee;
													preTaxDiscRate = floatCalculatePercentageU64(final_prod_amount, preTaxDiscRupee);
													final_discount_amt = preTaxDiscRupee;
													exit = 1;
												}
												else
												{
													final_discount_amt = 0;
													preTaxDiscRate = 0;
													clrscr();
													double_height = 1;
													gotoxy(3,1);
													lcdprint(" DISCOUNT SHOULD BE ");
													gotoxy(5,1);
													lcdprint(" LESS THAN FINAL AMT");
													beep(ERRRBEEP);
													Delay_ms(1000);
												}
											}
											else
											{
												final_discount_amt = 0;
												preTaxDiscRate = 0;
												exit = 1;
											}
										}

										vat_amount = 0;
										service_amount = 0;
										final_discount_amt = 0;

										managePreTaxDiscountFromRAMAndStore(startRec, preTaxDiscRate);

										final_prod_amount = 0;
										vat_amount = 0;
										service_amount = 0;
										sgstArrayPtr = 0;
										cgstArrayPtr = 0;
										recArrayPtr = 0;
										g_prodPosArrayPtr = 0;
										g_prodArrayPtr = 0;
										final_discount_amt = 0;
										for(loop = startRec, i = 0; loop < endRec; loop++)
										{
											get_rec(loop);

											if(mc.mc.udf_en_dis)
											{
												if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O'))
												{
													final_prod_amount += rec.rec.final_prod_amount;
													tempSlab = rec.rec.slab_no;
													if(rec.rec.sgst_amt)
													{
														vat_amount += rec.rec.sgst_amt;
														sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;

													}
													if(rec.rec.cgst_amt)
													{
														service_amount += rec.rec.cgst_amt;
														cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
													}
													for(i = 0; i < RECDAT_LEN; i++)
														recArray[recArrayPtr][i] = rec.arr[i];
													final_discount_amt += rec.rec.discount_amt;
													recArrayPtr++;
													g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
													prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
													prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
												}
											}
											else
											{
												if((rec.rec.bill_stat == 'O'))
												{
													final_prod_amount += rec.rec.final_prod_amount;
													if(rec.rec.sgst_amt)
													{
														vat_amount += rec.rec.sgst_amt;
														sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;

													}
													if(rec.rec.cgst_amt)
													{
														service_amount += rec.rec.cgst_amt;
														cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
													}
													for(i = 0; i < RECDAT_LEN; i++)
														recArray[recArrayPtr][i] = rec.arr[i];
													final_discount_amt += rec.rec.discount_amt;
													recArrayPtr++;
													g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
													prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
													prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
												}
											}
										}
									}

									sortVATAndServiceArray();		//Mrugnil 31/03/25

									final_discount_amt = pretax_discount_rupee;

									final_tot_amount = final_prod_amount;

									final_tot_amount -= final_discount_amt;
									if(!mc.mc.reverse_tax_en_dis)
									{
										final_tot_amount += vat_amount;
										final_tot_amount += service_amount;
									}
									if(config.config.other_charges == 1)
									{
										temp_amount = config.config.other_chrg_rate;
										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
										final_tot_amount = (final_tot_amount + service_tax);
									}
									break;
			case POST_TAX_PERCENT:
									final_tot_amount = final_prod_amount;
									if(!mc.mc.reverse_tax_en_dis)
									{
										final_tot_amount += vat_amount;
										final_tot_amount += service_amount;
									}

									if(config.config.other_charges == 1)
									{
										temp_amount = config.config.other_chrg_rate;
										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
										final_tot_amount = (final_tot_amount + service_tax);
									}
									clrscr();
									double_height = 1;
									gotoxy(1,1);
									lcdprint("ENTER DISCOUNT IN %");
									gotoxy(6,1);
									lcdprint("BILL:");
									value_hex_asc_64U = final_tot_amount;
									hex_asc_64U(asc, 12);
									rem_zeros(asc, 8);
									for(i = 0; i < 12; i++)
									{
										if(i == 10)
											disp_char('.');
										disp_char(asc[i]);
									}
									get_dig_dot_esc_key = 0;
									recordedDotPosition = 0;
									final_amt = get_dig_dot(3,9,2,2,0,1);
									if(!get_dig_dot_esc_key)
									{
										if(final_amt)
										{
											final_discount_amt = 0;
											final_discount_amt = floatExtractionU64(final_tot_amount, final_amt, DISCOUNT);
										}
										else
											final_discount_amt = 0;
									}
									else
										final_discount_amt = 0;

									final_tot_amount -= final_discount_amt;
									break;
			case   POST_TAX_RUPEE:
									final_tot_amount = final_prod_amount;
									if(!mc.mc.reverse_tax_en_dis)
									{
										final_tot_amount += vat_amount;
										final_tot_amount += service_amount;
									}

									if(config.config.other_charges == 1)
									{
										temp_amount = config.config.other_chrg_rate;
										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
										final_tot_amount = (final_tot_amount + service_tax);
									}
									exit = 0;
									while(!exit)
									{
										clrscr();
										double_height = 1;
										gotoxy(1,1);
										lcdprint("ENTER DISCOUNT IN Rs.");
										gotoxy(6,1);
										lcdprint("BILL:");
										value_hex_asc_64U = final_tot_amount;
										hex_asc_64U(asc, 12);
										rem_zeros(asc, 8);
										for(i = 0; i < 12; i++)
										{
											if(i == 10)
												disp_char('.');
											disp_char(asc[i]);
										}
										get_dig_dot_esc_key = 0;
										recordedDotPosition = 0;
										final_amt = get_dig_dot(3,9,5,2,0,1);
										if(!get_dig_dot_esc_key)
										{
											if(final_amt)
											{
												final_discount_amt = 0;
												if(final_amt < final_tot_amount)
												{
													final_discount_amt = final_amt;
													exit = 1;
												}
												else
													final_amt = 0;

											}
											else
											{
												final_discount_amt = 0;
												exit = 1;
											}
										}
										else
										{
											final_discount_amt = 0;
											exit = 1;
										}
									}
//									clrscr();
//									double_height = 1;
//									double_width = 0;
//									gotoxy(3,1);
//									lcdprint("   SAVING RECORDS   ");

									final_tot_amount -= final_discount_amt;
									break;
		}
	}
	else
	{
		if(!mc.mc.reverse_tax_en_dis)
		{
			final_tot_amount += vat_amount;
			final_tot_amount += service_amount;
		}
		if(config.config.other_charges == 1)
		{
			temp_amount = config.config.other_chrg_rate;
			service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
			final_tot_amount = (final_tot_amount + service_tax);
		}
	}

    if(mc.mc.parcel_en_dis==1)
    {
    	temp_parcel_amt = 0;
    	temp_parcel_rate = 0;

    	if((mc.mc.parcel_rate_amt == 1) && (udf_no > 225) && (udf_no <= 250))
    	{
    		clrscr();
			double_height = 1;
			gotoxy(1,1);
			lcdprint("ENT PARCEL CHARGE(%)");
			//hex_asc(temprec.temprec.final_prod_amount, asc, 9);
			hex_asc(final_tot_amount, asc, 9); //vikas
			rem_zeros(asc, 5);
			gotoxy(3,1);
			lcdprint("AMT: ");
			for(i = 0; i < 9; i++)
			{
				if(i == 7)
					disp_char('.');
				disp_char(asc[i]);
			}
			temp_parcel_rate = get_dig_dot(6,9,2,2,0,1);
			temp_parcel_amt = floatExtractionU64(final_tot_amount, temp_parcel_rate, TAX);
			//rec.rec.parcel_rate =  getDig(6,9,0,5,1,1);
    	}
    	else if((mc.mc.parcel_rate_amt == 2)&& (udf_no > 225) && (udf_no <= 250))
    	{
    		exit = 0;
    		while(!exit)
    		{
    			clrscr();
				double_height = 1;
				gotoxy(1,1);
				lcdprint("ENTER PARCEL CHARGES");
				//hex_asc(temprec.temprec.final_prod_amount, asc, 9);
				hex_asc(final_tot_amount, asc, 9);
				rem_zeros(asc, 5);
				gotoxy(3,1);
				lcdprint("AMT: ");
				for(i = 0; i < 9; i++)
				{
					if(i == 7)
						disp_char('.');
					disp_char(asc[i]);
				}
				temp_parcel_amt = get_dig_dot(6,9,5,2,0,1);
				//rec.rec.parcel_amt =  getDig(6,9,0,5,1,1);

				get_dig_dot_esc_key = 0;
				if(!get_dig_dot_esc_key)
				{
					if(temp_parcel_amt)
					{
						if(temp_parcel_amt <= final_tot_amount)
						{
							//rec.rec.parcel_amt = temp_parcel_amt;
							exit = 1;
						}
						else
						{
							clrscr();
							double_height = 1;
							gotoxy(2,1);
							lcdprint("PARCEL AMT EXCEEDING");
							gotoxy(4,1);
							lcdprint("   PRODUCT AMOUNT   ");
							beep(ERRRBEEP);
							Delay_ms(1000);
						}
					}
					else
					{
						//rec.rec.parcel_amt = 0;
						exit = 1;
					}
				}
				else
				{
					//rec.rec.parcel_amt = 0;
					exit = 1;
				}
    		}

    	}
        final_tot_amount += temp_parcel_amt;
    }

	sortVATAndServiceArray();

	if(returnBill)
		manageTAX366DaySummaryAndStore(0);
	else
		manageTAX366DaySummaryAndStore(1);

	if(returnBill)
	{
		if(flagIGST)
		{
			manageTAXFromRAMAndStore(IGST, 0);
		}
		else
		{
			manageTAXFromRAMAndStore(SGST, 0);
			manageTAXFromRAMAndStore(CGST, 0);
		}
	}
	else
	{
		if(flagIGST)
		{
			manageTAXFromRAMAndStore(IGST, 1);
		}
		else
		{
			manageTAXFromRAMAndStore(SGST, 1);
			manageTAXFromRAMAndStore(CGST, 1);
		}
	}

	if(mc.mc.stock_manage_en_dis)
	{
		if(returnBill)
			manageStockFronRAMAndStore(0);
		else
			manageStockFronRAMAndStore(1);
	}

	if(returnBill)
		manageProductSummaryFromRAMAndStore(0);
	else
		manageProductSummaryFromRAMAndStore(1);

	for(i = 0; i < RECDAT_LEN; i++)
		rec.arr[i] = 0;

	rec.rec.strt_byt = 'L';
    rec.rec.dd = tim.tim.dd;
    rec.rec.mm = tim.tim.mm;
    rec.rec.yy = tim.tim.yy;
    rec.rec.hr = tim.tim.hr;
    rec.rec.mn = tim.tim.mn;
    rec.rec.sc = tim.tim.sc;

    rec.rec.bill_no_char = mc.mc.bill_no_char;
	rec.rec.product_code = 0;
    rec.rec.rate = 0;
	rec.rec.quantity = final_quantity;
	rec.rec.unit_type = 0;
	rec.rec.discount_type = mc.mc.discount_type;

	rec.rec.payment_mode = paymentType;
	rec.rec.igstFlag = flagIGST;

	//store operator
	rec.rec.agent_no = operatorCode;

	if(mc.mc.udf_en_dis)
		rec.rec.rcpt_no = udfno.udfno.udfRcptNo;
	else
		rec.rec.rcpt_no	= mc.mc.rcpt_no;

	lastRecReceiptNo = rec.rec.rcpt_no;

	rec.rec.final_prod_amount = final_prod_amount;

//    if ((mc.mc.discount_en == ENABLE) && ((rec.rec.discount_type == POST_TAX_PERCENT) || (rec.rec.discount_type == POST_TAX_RUPEE)))
//        rec.rec.disc_rate = floatCalculatePercentageU64 ((final_prod_amount + vat_amount + service_amount), final_discount_amt); //vikas
//    else if ((mc.mc.discount_en == ENABLE) && ((rec.rec.discount_type == PRE_TAX) || (rec.rec.discount_type == PRE_TAX_RUPEE)))
//        rec.rec.disc_rate = floatCalculatePercentageU64 (final_prod_amount, final_discount_amt); //vikas
//
//	if ((mc.mc.discount_en == ENABLE) && ((rec.rec.discount_type == POST_TAX_RUPEE)))             //akshay 040620
//		rec.rec.disc_rate = floatCalculatePercentageU64 ((final_prod_amount + vat_amount + service_amount), final_discount_amt); //vikas
//	else if ((mc.mc.discount_en == ENABLE) && ((rec.rec.discount_type == PRE_TAX_RUPEE)))
//		rec.rec.disc_rate = floatCalculatePercentageU64 (final_prod_amount, final_discount_amt); //vikas
//	else if((mc.mc.discount_en == ENABLE ) && (rec.rec.discount_type == POST_TAX_PERCENT || rec.rec.discount_type == PRE_TAX))       //akshay 030620
//		rec.rec.disc_rate = final_amt;

	rec.rec.discount_amt = final_discount_amt;
	rec.rec.sgst_amt = vat_amount;
	rec.rec.cgst_amt = service_amount;
	rec.rec.total_amt = final_tot_amount;
	rec.rec.other_chrg = service_tax;
	rec.rec.other_chrg_rate = config.config.other_chrg_rate;

	if((mc.mc.discount_en == ENABLE ) && (rec.rec.discount_type == PRE_TAX || rec.rec.discount_type == ITEMWISE || rec.rec.discount_type == ITEMWISE_RUPEE || rec.rec.discount_type == PRE_TAX_RUPEE))		//Mrugnil 16/05/23
		rec.rec.disc_rate = (((UINT64)rec.rec.discount_amt * 10000)/rec.rec.final_prod_amount);		//Mrugnil 02/09/23
	else if((mc.mc.discount_en == ENABLE ) && (rec.rec.discount_type == POST_TAX_PERCENT || rec.rec.discount_type == POST_TAX_RUPEE))		//Mrugnil 16/05/23
		rec.rec.disc_rate = (((UINT64)rec.rec.discount_amt * 10000)/(rec.rec.final_prod_amount + baseTaxCalculator() + (UINT64)rec.rec.other_chrg));		//Mrugnil 02/09/23

	rec.rec.parcel_amt = temp_parcel_amt; //vikas0
    rec.rec.parcel_rate = temp_parcel_rate; //vikas5

	rec.rec.round_off_tot_amt = rec.rec.total_amt;
	rec.rec.bill_stat = 'O';
	if(mc.mc.return_bill_en_dis)
	{
		if(returnBill)
		{
			rec.rec.calc_stat = 'R';
		}
		else
			rec.rec.calc_stat = 0;
	}
	else
		rec.rec.calc_stat = 0;

	if(mc.mc.udf_en_dis)
	{
		rec.rec.udf_no = udf_no;
		rec.rec.slab_no = tempSlab;
		rec.rec.udf_char = udf_char;
	}
	else
	{
		rec.rec.udf_no = 0;
		rec.rec.slab_no = 1;
		rec.rec.udf_char = 0;
	}

	if(mc.mc.round_off)
	{
		pre_roundoff = rec.rec.total_amt;

		post_roundoff = round_off_func(mc.mc.round_off);

		if(post_roundoff)
			rec.rec.round_off_tot_amt = post_roundoff;
		else
			rec.rec.round_off_tot_amt = rec.rec.total_amt;
	}
	last_bill_amount = rec.rec.round_off_tot_amt;
	rec.rec.last = 1;

	if(mc.mc.cust_name_entry)
	{
		if(mc.mc.currentPG)
			cust_det = mc.mc.cust_details0;
		else
			cust_det = mc.mc.cust_details1;

		rec.rec.cust_detail_pos = cust_det;
	}
	else
		rec.rec.cust_detail_pos = 0;

	if(mc.mc.currentPG)
		store_rec(mc.mc.recs);
	else
		store_rec(mc.mc.rec1);

	saveSummaryDetails();

	udfrec.udfrec.udfActive = 1;
	store_udf_rec(udf_no, mc.mc.currentPG);

	get_rpt_details();
	get_rpt_details_count(rpt_details.rpt_details.bwr_ptr);

	bwr.bwr.bill_stat = 'C';
	bwr.bwr.calc_stat = rec.rec.calc_stat;
	bwr.bwr.dd = rec.rec.dd;
	bwr.bwr.mm = rec.rec.mm;
	bwr.bwr.yy = rec.rec.yy;
	bwr.bwr.hr = rec.rec.hr;
	bwr.bwr.mn = rec.rec.mn;
	bwr.bwr.sc = rec.rec.sc;
	bwr.bwr.final_prod_amount = rec.rec.final_prod_amount;
	bwr.bwr.cgst_amt = rec.rec.cgst_amt;
	bwr.bwr.sgst_amt = rec.rec.sgst_amt;
	bwr.bwr.rcpt_no = rec.rec.rcpt_no;
	bwr.bwr.round_off_tot_amt = rec.rec.round_off_tot_amt;
	bwr.bwr.returnedProduct = rec.rec.returnedProduct;
	bwr.bwr.strt_byt = rec.rec.strt_byt;
	bwr.bwr.last = rec.rec.last;
	bwr.bwr.discount_amt = rec.rec.discount_amt;
	bwr.bwr.other_chrg = rec.rec.other_chrg;
	bwr.bwr.udf_no = rec.rec.udf_no;
	store_bwr();

	if(mc.mc.currentPG)
		mc.mc.recs++;
	else
		mc.mc.rec1++;

	mc.mc.checkOffByte = 0;
	if(mc.mc.gprs_en_dis == RECORD_LOT)
		mc.mc.gprs_record_sent++;
	if(mc.mc.gsm_en_dis == RECORD_LOT)
		mc.mc.gsm_record_sent++;

	if(mc.mc.cust_name_entry)
	{
		if(mc.mc.currentPG)
			cust_det = mc.mc.cust_details0;
		else
			cust_det = mc.mc.cust_details1;

		if(cust_det < MAX_CUSTOMER_DETAILS)
		{
			cust_details.cust_details.receipt_no = rec.rec.rcpt_no;
			for(i = 0; i < CUSTOMER_NAME_LEN; i++)
				cust_details.cust_details.CUSTOMER_NAME[i] = g_customerName[i];
			for(i = 0; i < GSTIN_NO_LEN; i++)
				cust_details.cust_details.GSTIN[i] = g_GST_IN[i];

			store_cust_details(cust_det, mc.mc.currentPG);

			cust_det++;

			if(mc.mc.currentPG)
				mc.mc.cust_details0 = cust_det;
			else
				mc.mc.cust_details1 = cust_det;
		}
		else
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(1,1);
			lcdprint("MAX CUSTOMER DETAILS");
			gotoxy(3,1);
			lcdprint("REACHED. CANNOT SAVE");
			gotoxy(5,1);
			if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_CUST_NAME))
			{
				lcdprint("    CUSTOMER NAME   ");
				gotoxy(7,1);
			}
			if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_GSTIN))
			{
				lcdprint("    GSTIN NUMBER    ");
			}
			beep(ERRRBEEP);
			Delay_ms(2000);
		}
	}

	return 1;
}
//-------------------------------------------------------------------------
//unsigned char store_last_rec_returnBill(unsigned char billType)
//{
//	UINT32 loop = 0, preTaxDiscRate = 0, loopCount = 0, SUMMARY_START_ADD = 0, loopProdSumm = 0;
//	UINT64 final_amt = 0, final_discount_amt = 0, tempRate = 0, serv = 0, vat_temp = 0, preTaxDiscRupee = 0;
//	UINT64 final_prod_amount = 0, final_tot_amount = 0, service_tax = 0, vat_amount = 0, temp_amount = 0, service_amount = 0;
//	UCHAR temp_arr[PRODUCT_LEN] = {0}, i = 0, tempSlab = 0, exit = 0, entry = 0, j = 0, found = 0;
//	UINT32 Test_tax_amount = 0, final_quantity = 0;
//
//	if(checkForLastReturnRecordSaved(billType))
//		return 1;
//
//
//	if(billType)
//		returnBill = 1;
//	else
//		returnBill = 0;
//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint("   SAVING RECORDS   ");
//
//	//store date time
//	read_rtc();
//	rec.rec.total_amt = 0;
//
//	if(mc.mc.udf_en_dis)
//	{
//		get_udf(udf_no);
//		if(!billType)
//			startRec = udfno.udfno.udfStart;
//		if(mc.mc.currentPG)
//			endRec = mc.mc.recs;
//		else
//			endRec = mc.mc.rec1;
//	}
//	else
//	{
//		if(!billType)
//			startRec = mc.mc.rec_start;
//		else
//		{
//			if(g_returnCount && !g_actualCount)
//				startRec = mc.mc.rec_start;
//		}
//		if(mc.mc.currentPG)
//			endRec = mc.mc.recs;
//		else
//			endRec = mc.mc.rec1;
//	}
//
//	final_prod_amount = 0;
//	vat_amount = 0;
//	service_amount = 0;
//	sgstArrayPtr = 0;
//	cgstArrayPtr = 0;
//	recArrayPtr = 0;
//	g_prodPosArrayPtr = 0;
//	g_prodArrayPtr = 0;
//	final_discount_amt = 0;
//	final_quantity = 0;
//	for(loop = startRec, i = 0; loop < endRec; loop++)
//	{
//		get_rec(loop);
//
//		if(mc.mc.udf_en_dis)
//		{
//			if(billType)
//			{
//				if((rec.rec.final_prod_amount) && (rec.rec.udf_no == udf_no)
//						&& (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//				{
//					final_amt += rec.rec.final_prod_amount;
//					tempSlab = rec.rec.slab_no;
//					final_discount_amt += rec.rec.discount_amt;
//					vat_amount += rec.rec.sgst_amt;
//					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
//					service_amount += rec.rec.cgst_amt;
//					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
//					final_quantity++;
//					for(i = 0; i < RECDAT_LEN; i++)
//						recArray[recArrayPtr][i] = rec.arr[i];
//					recArrayPtr++;
//					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
//					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
//					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
//				}
//			}
//			else
//			{
//				if((rec.rec.final_prod_amount) && (rec.rec.udf_no == udf_no)
//						&& (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//				{
//					final_amt += rec.rec.final_prod_amount;
//					tempSlab = rec.rec.slab_no;
//					final_discount_amt += rec.rec.discount_amt;
//					vat_amount += rec.rec.sgst_amt;
//					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
//					service_amount += rec.rec.cgst_amt;
//					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
//					final_quantity++;
//					for(i = 0; i < RECDAT_LEN; i++)
//						recArray[recArrayPtr][i] = rec.arr[i];
//					recArrayPtr++;
//					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
//					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
//					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
//				}
//			}
//		}
//		else
//		{
//			if(billType)
//			{
//				if((rec.rec.final_prod_amount) && (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//				{
//					final_amt += rec.rec.final_prod_amount;
//					tempSlab = rec.rec.slab_no;
//					final_discount_amt += rec.rec.discount_amt;
//					vat_amount += rec.rec.sgst_amt;
//					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
//					service_amount += rec.rec.cgst_amt;
//					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
//
//					for(i = 0; i < RECDAT_LEN; i++)
//						recArray[recArrayPtr][i] = rec.arr[i];
//					recArrayPtr++;
//					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
//					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
//					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
//				}
//			}
//			else
//			{
//				if((rec.rec.final_prod_amount) && (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//				{
//					final_amt += rec.rec.final_prod_amount;
//					tempSlab = rec.rec.slab_no;
//					final_discount_amt += rec.rec.discount_amt;
//					vat_amount += rec.rec.sgst_amt;
//					sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
//					service_amount += rec.rec.cgst_amt;
//					cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
//
//					for(i = 0; i < RECDAT_LEN; i++)
//						recArray[recArrayPtr][i] = rec.arr[i];
//					recArrayPtr++;
//					g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
//					prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
//					prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;
//				}
//			}
//		}
//	}
//
//	if(final_amt)
//		final_prod_amount = final_amt;
//	else
//		final_prod_amount = 0;
//
//	final_tot_amount = final_prod_amount;
//
//	final_amt = 0;
//	if(mc.mc.discount_en == ENABLE)
//	{
//		switch(mc.mc.discount_type)
//		{
//			case	ITEMWISE_RUPEE:
//									final_amt = 0;
//									for(loop = startRec; loop < endRec; loop++)
//									{
//										get_rec(loop);
//
//										if(mc.mc.udf_en_dis)
//										{
//											if(billType)
//											{
//												if((rec.rec.discount_amt) && (rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O') && (rec.rec.last == 1))
//													final_amt += rec.rec.discount_amt;
//											}
//											else
//											{
//												if((rec.rec.discount_amt) && (rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O') && (rec.rec.last != 1))
//													final_amt += rec.rec.discount_amt;
//											}
//										}
//										else
//										{
//											if(billType)
//											{
//												if((rec.rec.discount_amt) && (rec.rec.bill_stat == 'O') && (rec.rec.last == 1))
//													final_amt += rec.rec.discount_amt;
//											}
//											else
//											{
//												if((rec.rec.discount_amt) && (rec.rec.bill_stat == 'O') && (rec.rec.last != 1))
//													final_amt += rec.rec.discount_amt;
//											}
//										}
//									}
//									final_discount_amt = final_amt;
//									final_tot_amount = (final_prod_amount - final_discount_amt);
//									if(!mc.mc.reverse_tax_en_dis)
//									{
//										final_tot_amount += vat_amount;
//										final_tot_amount += service_amount;
//									}
//									if(config.config.other_charges == 1)
//									{
//										temp_amount = config.config.other_chrg_rate;
//										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//										final_tot_amount = (final_tot_amount + service_tax);
//									}
//
//									break;
//			case 	     ITEMWISE:
//									final_tot_amount = final_prod_amount;
//
//									final_tot_amount -= final_discount_amt;
//									if(!mc.mc.reverse_tax_en_dis)
//									{
//										final_tot_amount += vat_amount;
//										final_tot_amount += service_amount;
//									}
//									if(config.config.other_charges == 1)
//									{
//										temp_amount = config.config.other_chrg_rate;
//										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//										final_tot_amount = (final_tot_amount + service_tax);
//									}
//									break;
//			case          PRE_TAX:
//									clrscr();
//									double_height = 1;
//									gotoxy(1,1);
//									lcdprint("ENTER DISCOUNT IN %");
//									get_dig_dot_esc_key = 0;
//									recordedDotPosition = 0;
//									final_amt = get_dig_dot(4,9,2,2,0,1);
//									if(!get_dig_dot_esc_key)
//									{
//										if(final_amt)
//										{
//											preTaxDiscRate = final_amt;
//										}
//										else
//										{
//											final_discount_amt = 0;
//											preTaxDiscRate = 0;
//										}
//									}
//									else
//									{
//										final_discount_amt = 0;
//										preTaxDiscRate = 0;
//									}
//									clrscr();
//									double_height = 1;
//									double_width = 0;
//									gotoxy(3,1);
//									lcdprint("   SAVING RECORDS   ");
//
//									vat_amount = 0;
//									service_amount = 0;
//									final_discount_amt = 0;
//
//									if(mc.mc.reverse_tax_en_dis)
//										final_prod_amount = 0;
//
//									for(loop = startRec; loop < endRec; loop++)
//									{
//										get_rec(loop);
//
//										if(mc.mc.udf_en_dis)
//										{
//											entry = 0;
//											if(billType)
//											{
//												if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											else
//											{
//												if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											if(entry)
//											{
//												rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);
//												final_discount_amt += rec.rec.discount_amt;
//
//												if(mc.mc.reverse_tax_en_dis)
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
//													}
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
//													}
//
//													final_prod_amount += rec.rec.final_prod_amount;
//
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//												else
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
//													}
//													else
//														rec.rec.sgst_amt = 0;
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
//													}
//													else
//														rec.rec.cgst_amt = 0;
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//											}
//										}
//										else
//										{
//											if(billType)
//											{
//												if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											else
//											{
//												if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											if(entry)
//											{
//												rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);
//												final_discount_amt += rec.rec.discount_amt;
//
//												if(mc.mc.reverse_tax_en_dis)
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
//													}
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
//													}
//
//													final_prod_amount += rec.rec.final_prod_amount;
//
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//												else
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
//													}
//													else
//														rec.rec.sgst_amt = 0;
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
//													}
//													else
//														rec.rec.cgst_amt = 0;
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//											}
//										}
//									}
//
//									final_tot_amount = final_prod_amount;
//
//									final_tot_amount -= final_discount_amt;
//									if(!mc.mc.reverse_tax_en_dis)
//									{
//										final_tot_amount += vat_amount;
//										final_tot_amount += service_amount;
//									}
//									if(config.config.other_charges == 1)
//									{
//										temp_amount = config.config.other_chrg_rate;
//										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//										final_tot_amount = (final_tot_amount + service_tax);
//									}
//									break;
//			case	PRE_TAX_RUPEE:
//									while(!exit)
//									{
//										final_amt = 0;
//										pretax_discount_rupee = 0;
//										clrscr();
//										double_height = 1;
//										gotoxy(1,1);
//										lcdprint("ENTER DISCOUNT IN Rs");
//										get_dig_dot_esc_key = 0;
//										recordedDotPosition = 0;
//										preTaxDiscRupee = get_dig_dot(4,9,5,2,0,1);
//										if(!get_dig_dot_esc_key)
//										{
//											if(preTaxDiscRupee < final_prod_amount)
//											{
//												pretax_discount_rupee = preTaxDiscRupee;
//												preTaxDiscRate = floatCalculatePercentageU64(final_prod_amount, preTaxDiscRupee);
//												final_discount_amt = preTaxDiscRupee;
//												exit = 1;
//											}
//											else
//											{
//												final_discount_amt = 0;
//												preTaxDiscRate = 0;
//												clrscr();
//												double_height = 1;
//												gotoxy(3,1);
//												lcdprint(" DISCOUNT SHOULD BE ");
//												gotoxy(5,1);
//												lcdprint(" LESS THAN FINAL AMT");
//												beep(ERRRBEEP);
//												Delay_ms(1000);
//											}
//										}
//										else
//										{
//											final_discount_amt = 0;
//											preTaxDiscRate = 0;
//											exit = 1;
//										}
//									}
//
//									vat_amount = 0;
//									service_amount = 0;
//									final_discount_amt = 0;
//
//									if(mc.mc.reverse_tax_en_dis)
//										final_prod_amount = 0;
//
//									for(loop = startRec; loop < endRec; loop++)
//									{
//										get_rec(loop);
//
//										if(mc.mc.udf_en_dis)
//										{
//											entry = 0;
//											if(billType)
//											{
//												if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											else
//											{
//												if((rec.rec.udf_no == udf_no) && (rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											if(entry)
//											{
//												rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);
//												final_discount_amt += rec.rec.discount_amt;
//
//												if(mc.mc.reverse_tax_en_dis)
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
//													}
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
//													}
//
//													final_prod_amount += rec.rec.final_prod_amount;
//
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//												else
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
//													}
//													else
//														rec.rec.sgst_amt = 0;
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
//													}
//													else
//														rec.rec.cgst_amt = 0;
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//											}
//										}
//										else
//										{
//											if(billType)
//											{
//												if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct == 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											else
//											{
//												if((rec.rec.bill_stat == 'O') && (rec.rec.returnedProduct != 1))
//													entry = 1;
//												else
//													entry = 0;
//											}
//											if(entry)
//											{
//												rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);
//												final_discount_amt += rec.rec.discount_amt;
//
//												if(mc.mc.reverse_tax_en_dis)
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
//													}
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
//													}
//
//													final_prod_amount += rec.rec.final_prod_amount;
//
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//												else
//												{
//													if(rec.rec.prd_sgst)
//													{
//														rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
//													}
//													else
//														rec.rec.sgst_amt = 0;
//													if(rec.rec.prd_cgst)
//													{
//														rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, TAX);
//													}
//													else
//														rec.rec.cgst_amt = 0;
//													vat_amount += rec.rec.sgst_amt;
//													service_amount += rec.rec.cgst_amt;
//													store_rec(loop);
//												}
//											}
//										}
//									}
//
//									final_discount_amt = pretax_discount_rupee;
//
//									final_tot_amount = final_prod_amount;
//
//									final_tot_amount -= final_discount_amt;
//									if(!mc.mc.reverse_tax_en_dis)
//									{
//										final_tot_amount += vat_amount;
//										final_tot_amount += service_amount;
//									}
//									if(config.config.other_charges == 1)
//									{
//										temp_amount = config.config.other_chrg_rate;
//										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//										final_tot_amount = (final_tot_amount + service_tax);
//									}
//									break;
//			case POST_TAX_PERCENT:
//									final_tot_amount = final_prod_amount;
//									if(!mc.mc.reverse_tax_en_dis)
//									{
//										final_tot_amount += vat_amount;
//										final_tot_amount += service_amount;
//									}
//
//									if(config.config.other_charges == 1)
//									{
//										temp_amount = config.config.other_chrg_rate;
////										service_tax = (final_prod_amount * temp_amount) / 10000;
//										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//										final_tot_amount = (final_tot_amount + service_tax);
//									}
//									clrscr();
//									double_height = 1;
//									gotoxy(1,1);
//									lcdprint("ENTER DISCOUNT IN %");
//									gotoxy(6,1);
//									lcdprint("BILL:");
//									value_hex_asc_64U = final_tot_amount;
//									hex_asc_64U(asc, 12);
//									rem_zeros(asc, 8);
//									for(i = 0; i < 12; i++)
//									{
//										if(i == 10)
//											disp_char('.');
//										disp_char(asc[i]);
//									}
//									get_dig_dot_esc_key = 0;
//									recordedDotPosition = 0;
//									final_amt = get_dig_dot(3,9,2,2,0,1);
//									if(!get_dig_dot_esc_key)
//									{
//										if(final_amt)
//										{
//											final_discount_amt = 0;
////											final_discount_amt = (final_prod_amount * final_amt) / 10000;
//											final_discount_amt = floatExtractionU64(final_prod_amount, final_amt, DISCOUNT);
//										}
//										else
//											final_discount_amt = 0;
//									}
//									else
//										final_discount_amt = 0;
//
//									final_tot_amount -= final_discount_amt;
//									break;
//			case   POST_TAX_RUPEE:
////									get_mc();
//									final_tot_amount = final_prod_amount;
//									if(!mc.mc.reverse_tax_en_dis)
//									{
//										final_tot_amount += vat_amount;
//										final_tot_amount += service_amount;
//									}
//
//									if(config.config.other_charges == 1)
//									{
//										temp_amount = config.config.other_chrg_rate;
//										service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//										final_tot_amount = (final_tot_amount + service_tax);
//									}
//									exit = 0;
//									while(!exit)
//									{
//										clrscr();
//										double_height = 1;
//										gotoxy(1,1);
//										lcdprint("ENTER DISCOUNT IN Rs.");
//										gotoxy(6,1);
//										lcdprint("BILL:");
//										value_hex_asc_64U = final_tot_amount;
//										hex_asc_64U(asc, 12);
//										rem_zeros(asc, 8);
//										for(i = 0; i < 12; i++)
//										{
//											if(i == 10)
//												disp_char('.');
//											disp_char(asc[i]);
//										}
//										get_dig_dot_esc_key = 0;
//										recordedDotPosition = 0;
//										final_amt = get_dig_dot(3,9,5,2,0,1);
//										if(!get_dig_dot_esc_key)
//										{
//											if(final_amt)
//											{
//												final_discount_amt = 0;
//												if(final_amt < final_tot_amount)
//												{
//													final_discount_amt = final_amt;
//													exit = 1;
//												}
//												else
//													final_amt = 0;
//
//											}
//											else
//											{
//												final_discount_amt = 0;
//												exit = 1;
//											}
//										}
//										else
//										{
//											final_discount_amt = 0;
//											exit = 1;
//										}
//									}
//									clrscr();
//									double_height = 1;
//									double_width = 0;
//									gotoxy(3,1);
//									lcdprint("   SAVING RECORDS   ");
//
//									final_tot_amount -= final_discount_amt;
//									break;
//		}
//	}
//	else
//	{
//		if(!mc.mc.reverse_tax_en_dis)
//		{
//			final_tot_amount += vat_amount;
//			final_tot_amount += service_amount;
//		}
//		if(config.config.other_charges == 1)
//		{
//			temp_amount = config.config.other_chrg_rate;
//			service_tax = floatExtractionU64(final_prod_amount, temp_amount, TAX);
//			final_tot_amount = (final_tot_amount + service_tax);
//		}
//	}
//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint("   SAVING RECORDS   ");
//
//	sortVATAndServiceArray();
//
//	if(!returnBill)
//	{
//		if(flagIGST)
//		{
//			manageTAXFromRAMAndStore(IGST, 1);
//		}
//		else
//		{
//			manageTAXFromRAMAndStore(SGST, 1);
//			manageTAXFromRAMAndStore(CGST, 1);
//		}
//	}
//
//	if(!returnBill)
//	{
//		manageProductSummaryFromRAMAndStore(1);
//	}
//
//	for(i = 0; i < RECDAT_LEN; i++)
//		rec.arr[i] = 0;
//
//	rec.rec.strt_byt = 'L';
//    rec.rec.dd = tim.tim.dd;
//    rec.rec.mm = tim.tim.mm;
//    rec.rec.yy = tim.tim.yy;
//    rec.rec.hr = tim.tim.hr;
//    rec.rec.mn = tim.tim.mn;
//    rec.rec.sc = tim.tim.sc;
//
//	rec.rec.product_code = 0;
//    rec.rec.rate = 0;
//	rec.rec.quantity = final_quantity;
//	rec.rec.unit_type = 0;
//	rec.rec.discount_type = mc.mc.discount_type;
//
//	if(billType)
//		rec.rec.payment_mode = 1;
//	else
//		rec.rec.payment_mode = paymentType;
//
//	//store operator
//	rec.rec.agent_no = operatorCode;
//
//	if(mc.mc.udf_en_dis)
//		rec.rec.rcpt_no = udfno.udfno.udfRcptNo;
//	else
//		rec.rec.rcpt_no	= mc.mc.rcpt_no;
//
//	lastRecReceiptNo = rec.rec.rcpt_no;
//
//	rec.rec.final_prod_amount = final_prod_amount;
//	rec.rec.discount_amt = final_discount_amt;
//	rec.rec.sgst_amt = vat_amount;
//	rec.rec.cgst_amt = service_amount;
//	rec.rec.total_amt = final_tot_amount;
//	rec.rec.other_chrg = service_tax;
//	rec.rec.other_chrg_rate = config.config.other_chrg_rate;
//	rec.rec.round_off_tot_amt = rec.rec.total_amt;
//	rec.rec.returnedProduct = billType;
//
//	rec.rec.bill_stat = 'O';
//	if(mc.mc.return_bill_en_dis)
//	{
//		if(returnBill)
//		{
//			rec.rec.calc_stat = 'R';
//		}
//		else
//			rec.rec.calc_stat = 0;
//	}
//	else
//		rec.rec.calc_stat = 0;
//
//	if(mc.mc.udf_en_dis)
//	{
//		rec.rec.udf_no = udf_no;
//		rec.rec.slab_no = tempSlab;
//		rec.rec.udf_char = udf_char;
//	}
//	else
//	{
//		rec.rec.udf_no = 0;
//		rec.rec.slab_no = 1;
//		rec.rec.udf_char = 0;
//	}
//
//	if(mc.mc.round_off)
//	{
//		pre_roundoff = rec.rec.total_amt;
//
//		post_roundoff = round_off_func(mc.mc.round_off);
//
//		if(post_roundoff)
//			rec.rec.round_off_tot_amt = post_roundoff;
//		else
//			rec.rec.round_off_tot_amt = rec.rec.total_amt;
//	}
//
//	rec.rec.last = 1;
//
//	if(mc.mc.currentPG)
//		store_rec(mc.mc.recs);
//	else
//		store_rec(mc.mc.rec1);
//
//	if(!billType)
//		saveSummaryDetails();
//
//	udfrec.udfrec.udfActive = 1;
//	store_udf_rec(udf_no, mc.mc.currentPG);
//
//	bwr.bwr.bill_stat = 'C';
//	bwr.bwr.calc_stat = rec.rec.calc_stat;
//	bwr.bwr.dd = rec.rec.dd;
//	bwr.bwr.mm = rec.rec.mm;
//	bwr.bwr.yy = rec.rec.yy;
//	bwr.bwr.hr = rec.rec.hr;
//	bwr.bwr.mn = rec.rec.mn;
//	bwr.bwr.sc = rec.rec.sc;
//	bwr.bwr.final_prod_amount = rec.rec.final_prod_amount;
//	bwr.bwr.cgst_amt = rec.rec.cgst_amt;
//	bwr.bwr.sgst_amt = rec.rec.sgst_amt;
//	bwr.bwr.rcpt_no = rec.rec.rcpt_no;
//	bwr.bwr.round_off_tot_amt = rec.rec.round_off_tot_amt;
//	bwr.bwr.returnedProduct = rec.rec.returnedProduct;
//	bwr.bwr.strt_byt = rec.rec.strt_byt;
//	bwr.bwr.last = rec.rec.last;
//	bwr.bwr.discount_amt = rec.rec.discount_amt;
//	bwr.bwr.other_chrg = rec.rec.other_chrg;
//	bwr.bwr.udf_no = rec.rec.udf_no;
//	store_bwr();
//
//	if(mc.mc.currentPG)
//		mc.mc.recs++;
//	else
//		mc.mc.rec1++;
//
//	mc.mc.checkOffByte = 0;
//	store_mc();
//
//	if(mc.mc.gprs_en_dis == RECORD_LOT)
//		mc.mc.gprs_record_sent++;
//	if(mc.mc.gsm_en_dis == RECORD_LOT)
//		mc.mc.gsm_record_sent++;
//
//	store_mc();
//
//	return 1;
//}
//-------------------------------------------------------------------------
void restoreMastersDefault(void)
{
	UCHAR PWD[AGENT_PASSWORD_LEN], psdBuff[AGENT_PASSWORD_LEN], entry = 0, j = 0;
	UINT32 i = 0, t = 0;

	memset(PWD, ' ', AGENT_PASSWORD_LEN);
//	FlashReadData((((UINT32)AGENT_PG * PGBYTS) + ((UINT32) 1 * AGENT_TOTAL_LEN)), membuf, AGENT_TOTAL_LEN);
//	if(membuf[AGENT_FLAG] == '1')
//	{
//		for(j = 0; j < AGENT_PASSWORD_LEN; j++)
//			PWD[j] = membuf[j + AGENT_PASSWORD];
//		entry = 1;
//	}

	if((yes_no("   RESTORE MASTERS? ")))
	{
		entry = 0;
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("   ENTER PASSWORD   ");
		double_height = 0;
		double_width = 0;
		gotoxy(8,1);
		lcdprint("PRESS        ENT/CNL");

		if(getAlphaDig(5,12, psdBuff,AGENT_PASSWORD_LEN, 1, 1))
		{
			if((!strncmp(psdBuff,"030609", AGENT_PASSWORD_LEN)))
			{
				entry = 1;
			}
			else
			{
				clrscr();
				double_height = 1;
				gotoxy(3,1);
				lcdprint("AUTHENTICATION FAIL!");
				beep(ERRRBEEP);
				Delay_ms(2000);
				entry = 0;
			}
		}
		else
		{
			entry = 0;
		}
		if(entry)
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3,1);
			lcdprint("  RESTORING MASTERS ");
			double_height = 0;
			gotoxy(6,1);
			lcdprint("   PLEASE WAIT...   ");
			beep(LONGBEEP);

			FlashModifyData(((unsigned long)(SGST_SUMMARY_PG * PGBYTS)), g_8K_membuf, 4095);
			FlashModifyData(((unsigned long)(SGST_SUMMARY1_PG * PGBYTS)), g_8K_membuf, 4095);

			gotoxy(5,1);
			lcdprint("       PRODUCT      ");
			FlashReadData((((UINT32) UNIT_TYPE_PG * PGBYTS) + ((UINT32) 1 * UNIT_TYPE_LEN) + UNIT_TYPE_SERIAL_NO), asc, UNIT_TYPE_SERIAL_NO_LEN);
			FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) 1 * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			membuf[PRODUCT_UNIT_TYPE] = asc[0];
			membuf[PRODUCT_UNIT_TYPE + 1] = asc[1];
			membuf[PRODUCT_SER_NO]     = '0';
			membuf[PRODUCT_SER_NO + 1] = '0';
			membuf[PRODUCT_SER_NO + 2] = '0';
			membuf[PRODUCT_SER_NO + 3] = '1';
			membuf[PRODUCT_EN_FLAG]    = ',';
			FlashModifyData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) 1 * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			membuf[0] = END_CHR;
			FlashModifyData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) 2 * PRODUCT_LEN)), membuf, 1);
			mc.mc.prod_totalcount = count_prod();
			store_mc();
//			loadProductStockIntoStockUnionPages();
			product_stock.product_stock.totalStock = 0;
			product_stock.product_stock.sign = 0;
			store_product_stock(1);
			clearProductSummaryForSelectedDay(mc.mc.currentPG);
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3,1);
			lcdprint("  RESTORING MASTERS ");
			double_height = 0;
			gotoxy(6,1);
			lcdprint("   PLEASE WAIT...   ");
			beep(LONGBEEP);
			gotoxy(6,1);
			lcdprint("        UNITS       ");
			membuf[0] = END_CHR;
			FlashModifyData((((UINT32) UNIT_TYPE_PG * PGBYTS) + ((UINT32) 2 * UNIT_TYPE_LEN)), membuf, 1);
			mc.mc.total_units = count_unit();
			store_mc();

			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3,1);
			lcdprint("  RESTORING MASTERS ");
			double_height = 0;
			gotoxy(6,1);
			lcdprint("   PLEASE WAIT...   ");
			beep(LONGBEEP);
			gotoxy(6,1);
			lcdprint("      OPERATORS     ");
			membuf[0] = END_CHR;
			FlashModifyData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) 2 * AGENT_TOTAL_LEN)), membuf, 1);
			mc.mc.operator_totalcount = count_Agent();
			store_mc();

			erase_recs();
			clrscr();
			gotoxy(4,1);
			lcdprint("     LOADING .... ");
			double_height=0;
			gotoxy(7,1);
			lcdprint("    Please Wait !");

			mc.mc.rcpt_no=1;
			mc.mc.kot_no = 1;
			mc.mc.buzzer_en_dis=1;
			mc.mc.twoCopyBill	= 0;
			mc.mc.recs = 1;
			mc.mc.rec1 = 1;
			mc.mc.checkStartRecAddress = 0;
			mc.mc.billNoType = 0;
			mc.mc.paymode = 0;
			mc.mc.set_current_dd = 0;
			mc.mc.set_current_mm = 0;
			mc.mc.set_current_yy = 0;
			mc.mc.retail_bill_opt = 0;
			mc.mc.cash_reception_enable = 0;
			mc.mc.modify_rate_enable = 0;
			mc.mc.round_off = 0;
			mc.mc.cust_details_rpt = 0;
			mc.mc.bill_format_selection = 0;
			mc.mc.rec_start = 0;
			mc.mc.selected = 0;
			mc.mc.sale_amount = 0;
			mc.mc.discount_en = 0;
			mc.mc.discount_type = 0;
			mc.mc.current_hr = 0;
			mc.mc.prnLogo = 0;
			mc.mc.udf_en_dis = 0;
			mc.mc.max_table = 50;
			mc.mc.default_slab = 1;
			mc.mc.slab_totalcount = 0;
			mc.mc.kot_en_dis = 0;
			mc.mc.coupon_en_dis = 0;
			mc.mc.rec1 = 1;
			mc.mc.calculatorBillNo = 1;
			mc.mc.cust_name_entry = 0;
			mc.mc.stock_manage_en_dis = 0;
			mc.mc.reverse_tax_en_dis = 0;
			mc.mc.rupeeFlag = 1;
			mc.mc.report_save = 0;
			mc.mc.report_clerk = 0;
			mc.mc.currentPG = 0;
			mc.mc.return_bill_en_dis = 0;
			mc.mc.languageSett = 0;
			mc.mc.drawer_en_dis = 0;
			mc.mc.calculator_swap = 1;
			mc.mc.word_amount = 0;
			mc.mc.weighing_scale = 0;
			mc.mc.baudrate = 1;
			mc.mc.massStrg_barcd_USBKbd_sett = 0;
			mc.mc.net_amt_height = DH;
			mc.mc.operator_details_en_dis = 0;
			mc.mc.current_dd = 0;
			mc.mc.current_mm = 0;
			mc.mc.current_yy = 0;
			mc.mc.gprs_en_dis = 0;
			mc.mc.gsm_en_dis = 0;
			mc.mc.gprs_recordsLot = 10;
			mc.mc.gprs_record_sent = 0;
			mc.mc.gsm_record_sent = 0;
			mc.mc.gsm_recordsLot = 10;
			mc.mc.GPRS_rec_start = 1;
			mc.mc.gsm_interval_data = 0;
			mc.mc.gprs_interval_data = 0;
			mc.mc.current_hr = tim.tim.hr;
			mc.mc.print_tax_splitup = 1;
			mc.mc.hsn_en_dis = 0;
			mc.mc.modify_rate_enable = 0;
			mc.mc.cust_details0 = 1;
			mc.mc.cust_details1 = 1;
			mc.mc.actualRcptNo = 1;
			mc.mc.billKotseries = DIFFERENT_SERIES;
			mc.mc.time_en_dis = 1;
			mc.mc.srno_en_dis = 1;
			mc.mc.bill_no_char = 0;
			mc.mc.font_height = DH;
			mc.mc.checkIntervalFail = 0;
			mc.mc.lastMissedCallCount = 0;
			mc.mc.qty_rate_swap = QTY_RATE;
			mc.mc.qty_enter_method = QTY_3_3;
			mc.mc.digitsBeforeDot = 3;
			mc.mc.digitsAfterDot = 3;
			mc.mc.barcode_qty_en_dis = 0;
			mc.mc.product_selection = BEFORE_QTY;
			mc.mc.parcel_en_dis = 0;
			mc.mc.parcel_rate_amt = 0;//vikas
			mc.mc.weighing_scale_mode = 1; //vikas
			mc.mc.iw_disc_percent_auto_manual = 0;
			mc.mc.kot_qty_amt_print = 0;
			mc.mc.on_off_bill_wise_sms = 0;
			mc.mc.item_qty_print = 1;	//akshay 051019
			strncpy(mc.mc.billno_field, "BILL NO   ", BILL_NAME_LEN);

			mc.mc.lastTicket = 1;
			mc.mc.stock_minus = 1;
			mc.mc.last_amt_display_en_dis = 1;
			store_mc();

			clrscr();
			double_height = 1;
			gotoxy(1,1);
			lcdprint("   SET BILL NUMBER  ");
			double_height = 0;
			gotoxy(8,1);
			lcdprint("PRESS        ENT/CNL");
			double_height = 1;
			get_dig_esc_flag = 0;
			i = get_dig(4, 7, 5, mc.mc.rcpt_no, 1, 0);

			if((!get_dig_esc_flag) && (i > 0) && (i < MAX_RCPTNO))
			{
				clrscr();
				gotoxy(3,1);
				lcdprint(" SAVING BILL NUMBER ");
				double_height = 0;
				gotoxy(6,1);
				lcdprint("   PLEASE WAIT...   ");
				mc.mc.rcpt_no = i;
				store_mc();
			}

			hex_asc(mc.mc.udf_en_dis, asc,CONFIG_FLAG_LEN);
			FlashModifyData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_FLAG), asc, CONFIG_FLAG_LEN);

			hex_asc(mc.mc.kot_en_dis, asc,CONFIG_FLAG_LEN);
			FlashModifyData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_KOT_FLAG), asc, CONFIG_FLAG_LEN);

			hex_asc(mc.mc.max_table, asc,MAX_TABLES_LEN);
			FlashModifyData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_MAX_TABLES), asc, MAX_TABLES_LEN);

			changeBaudRate(BR115200);

			for(i = 0; i < DAILY_COLL_LEN; i++)
				daily.arr[i] = 0;

			for(i = 0; i < MONTHLY_COLL_LEN; i++)
				monthly.arr[i] = 0;

			for(i = 0; i < YEARLY_COLL_LEN; i++)
				yearly.arr[i] = 0;

		    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

			for(i = 0; i < 3; i++)
			{
				FlashModifyData((((unsigned long)(DAILY_COLL_BASE_ADDR * PGBYTS)) + ((unsigned long)(i * 4096))), g_8K_membuf, 4095);
			}

			for(i = 0; i < 9; i++)
			{
				FlashModifyData((((unsigned long)(MONTHLY_COLL_BASE_ADDR * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
			}

			FlashModifyData(((unsigned long)(YEARLY_COLL_BASE_ADDR * PGBYTS)), g_8K_membuf, 255);

			for(i = 0; i < 3; i++)
			{
				FlashModifyData((((unsigned long)(SLAB_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
			}

			for(i = 0; i < 9; i++)
			{
				FlashModifyData((((unsigned long)(UDF_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
			}

			FlashModifyData(((unsigned long)(HOT_KEY_PG * PGBYTS)), g_8K_membuf, 255);

			for(i = 0; i < mc.mc.sgst_summ_count; i++)
			{
				get_sgst_summ(i, mc.mc.currentPG);
				sgst_summary.sgst_summary.total_amount = 0;
				store_sgst_summ(i, 1);
				store_sgst_summ(i, 0);
			}

			for(i = 0; i < mc.mc.cgst_summ_count; i++)
			{
				get_cgst_summ(i, mc.mc.currentPG);
				cgst_summary.cgst_summary.total_amount = 0;
				store_cgst_summ(i, 1);
				store_cgst_summ(i, 0);
			}

			for(i = 0; i < mc.mc.igst_summ_count; i++)
			{
				get_igst_summ(i, mc.mc.currentPG);
				igst_summary.igst_summary.total_amount = 0;
				store_igst_summ(i, 1);
				store_igst_summ(i, 0);
			}

//			for(i = 0; i < RPT_PAGE_UNION_LEN; i++)		//pending
//				rpt_details.arr[i] = 0;

			memset(g_8K_membuf,0,sizeof(g_8K_membuf));
			FlashModifyData((((unsigned long)(REPORT_DETAILS_COUNT_PG * PGBYTS))), g_8K_membuf, RPT_PAGE_UNION_LEN * 370);

//			rpt_details_count.rpt_details_count.bwr_count = 0;
//			rpt_details_count.rpt_details_count.iwr_count = 0;
//			for(i = 0; i <= 366; i++)
//				store_rpt_details_count(i);

			rpt_details.rpt_details.bwr_ptr = 1;
			rpt_details.rpt_details.iwr_ptr = 1;
			store_rpt_details();

//			for(i = 0; i < TWR_UNION_LEN; i++)
//				twr.arr[i] = 0;
//
//			for(t = 0; t <= 366; t++)
//				store_twr(t);

			memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
			for(i = 0; i < 7; i++)
			{
				FlashModifyData(((unsigned long)(TWR_1_TO_15DAYS * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
			}
			FlashModifyData(((unsigned long)(TWR_1_TO_15DAYS * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 928);

			twr_ptr.twr_ptr.day_ptr = 1;
			store_twr_ptr();//vikas

			FlashModifyData((((UINT32)CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME),  "TABLE NO            ", UDF_NAME_LEN);

			for(i = 0; i < 2; i++)
			{
				FlashModifyData((((unsigned long)(UDF_PRD_CNT_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
			}

			FlashModifyData(((unsigned long)(UDF_REC_PG1 * PGBYTS)), g_8K_membuf, 255);
			FlashModifyData(((unsigned long)(UDF_REC_PG2 * PGBYTS)), g_8K_membuf, 255);

			loadConfigDetails();

			load256ProductSummaryinTempArray(1);
			loadProductsStockTempArray(1);
			loadTaxSummaryinTempArray(0, mc.mc.sgst_summ_count, SGST, mc.mc.currentPG);
			loadTaxSummaryinTempArray(0, mc.mc.cgst_summ_count, CGST, mc.mc.currentPG);
			loadTaxSummaryinTempArray(0, mc.mc.igst_summ_count, IGST, mc.mc.currentPG);
			sendEraseStatus(0, voidRecs, voidTotalAmount);
		}
	}
}
//-------------------------------------------------------------------------
void restoreDefaults(void)
{
	UINT32 i = 0, j = 0, t = 0;
	UCHAR PWD[AGENT_PASSWORD_LEN], psdBuff[AGENT_PASSWORD_LEN], entry = 0;
	UINT32 tempReceiptNo = 0;

	memset(PWD, ' ', AGENT_PASSWORD_LEN);
	FlashReadData((((UINT32)AGENT_PG * PGBYTS) + ((UINT32) 1 * AGENT_TOTAL_LEN)), membuf, AGENT_TOTAL_LEN);
	if(membuf[AGENT_FLAG] == '1')
	{
		for(j = 0; j < AGENT_PASSWORD_LEN; j++)
			PWD[j] = membuf[j + AGENT_PASSWORD];
		entry = 1;
	}

	if((yes_no("  RESTORE DEFAULTS? ")) && (entry))
	{
		entry = 0;
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("   ENTER PASSWORD   ");
		double_height = 0;
		double_width = 0;
		gotoxy(8,1);
		lcdprint("PRESS        ENT/CNL");

		if(getAlphaDig(5,12, psdBuff,AGENT_PASSWORD_LEN, 1, 1))
		{
			if((!strncmp(psdBuff,PWD, AGENT_PASSWORD_LEN)))
			{
				entry = 1;
			}
			else
			{
				clrscr();
				double_height = 1;
				gotoxy(3,1);
				lcdprint("AUTHENTICATION FAIL!");
				beep(ERRRBEEP);
				Delay_ms(2000);
				entry = 0;
			}
		}
		else
		{
			entry = 0;
		}

		if(entry)
		{
			updateFlashContents();

			erase_recs();
			clrscr();
			gotoxy(4,1);
			lcdprint("     LOADING .... ");
			double_height=0;
			gotoxy(7,1);
			lcdprint("    Please Wait !");

			mc.mc.rec_start = 0;
			mc.mc.selected = 0;
			mc.mc.rcpt_no=1;
			mc.mc.kot_no = 1;
			mc.mc.currentPG = 0;
			mc.mc.calculatorBillNo = 1;
			mc.mc.current_hr = 0;
			mc.mc.set_current_dd = 0;
			mc.mc.set_current_mm = 0;
			mc.mc.set_current_yy = 0;
			mc.mc.current_dd = 0;
			mc.mc.current_mm = 0;
			mc.mc.current_yy = 0;
			mc.mc.sale_amount = 0;
			mc.mc.recs = 1;
			mc.mc.rec1 = 1;
			mc.mc.cust_details0 = 1;
			mc.mc.cust_details1 = 1;
			mc.mc.actualRcptNo = 1;
			store_mc();

			for(i = 0; i < DAILY_COLL_LEN; i++)
				daily.arr[i] = 0;

			for(i = 0; i < MONTHLY_COLL_LEN; i++)
				monthly.arr[i] = 0;

			for(i = 0; i < YEARLY_COLL_LEN; i++)
				yearly.arr[i] = 0;

		    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

			for(i = 0; i < 3; i++)
			{
				FlashModifyData((((unsigned long)(DAILY_COLL_BASE_ADDR * PGBYTS)) + ((unsigned long)(i * 4096))), g_8K_membuf, 4095);
			}

			for(i = 0; i < 9; i++)
			{
				FlashModifyData((((unsigned long)(MONTHLY_COLL_BASE_ADDR * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
			}

			FlashModifyData(((unsigned long)(YEARLY_COLL_BASE_ADDR * PGBYTS)), g_8K_membuf, 255);

			for(i = 0; i < 6; i++)
			{
				FlashModifyData(((unsigned long)(PRODUCT_SUMMARY_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
				FlashModifyData(((unsigned long)(PRODUCT_SUMMARY1_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
			}

			FlashModifyData(((unsigned long)(PRODUCT_SUMMARY_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 1024);
			FlashModifyData(((unsigned long)(PRODUCT_SUMMARY1_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 1024);

			for(i = 0; i < mc.mc.sgst_summ_count; i++)
			{
				get_sgst_summ(i, mc.mc.currentPG);
				sgst_summary.sgst_summary.total_amount = 0;
				store_sgst_summ(i, 1);
				store_sgst_summ(i, 0);
			}

			for(i = 0; i < mc.mc.cgst_summ_count; i++)
			{
				get_cgst_summ(i, mc.mc.currentPG);
				cgst_summary.cgst_summary.total_amount = 0;
				store_cgst_summ(i, 1);
				store_cgst_summ(i, 0);
			}

			for(i = 0; i < mc.mc.igst_summ_count; i++)
			{
				get_igst_summ(i, mc.mc.currentPG);
				igst_summary.igst_summary.total_amount = 0;
				store_igst_summ(i, 1);
				store_igst_summ(i, 0);
			}

//			for(i = 0; i < RPT_PAGE_UNION_LEN; i++)		//pending
//				rpt_details.arr[i] = 0;

			memset(g_8K_membuf,0,sizeof(g_8K_membuf));
			FlashModifyData((((unsigned long)(REPORT_DETAILS_COUNT_PG * PGBYTS))), g_8K_membuf, RPT_PAGE_UNION_LEN * 370);

//			rpt_details_count.rpt_details_count.bwr_count = 0;
//			rpt_details_count.rpt_details_count.iwr_count = 0;
//			for(i = 0; i <= 366; i++)
//				store_rpt_details_count(i);

			rpt_details.rpt_details.bwr_ptr = 1;
			rpt_details.rpt_details.iwr_ptr = 1;
			store_rpt_details();

//			for(i = 0; i < TWR_UNION_LEN; i++)
//				twr.arr[i] = 0;
//
//			for(t = 0; t <= 366; t++)
//				store_twr(t);

			memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
			for(i = 0; i < 7; i++)
			{
				FlashModifyData(((unsigned long)(TWR_1_TO_15DAYS * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
			}
			FlashModifyData(((unsigned long)(TWR_1_TO_15DAYS * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 928);

			twr_ptr.twr_ptr.day_ptr = 1;
			store_twr_ptr();

			for(i = 0; i < 2; i++)
			{
				FlashModifyData((((unsigned long)(UDF_PRD_CNT_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
			}

			FlashModifyData(((unsigned long)(UDF_REC_PG1 * PGBYTS)), g_8K_membuf, 255);
			FlashModifyData(((unsigned long)(UDF_REC_PG2 * PGBYTS)), g_8K_membuf, 255);

			loadConfigDetails();

			set_dattim(0);
			clrscr();
			double_height = 1;
			gotoxy(1,1);
			lcdprint("   SET BILL NUMBER  ");
			double_height = 0;
			gotoxy(8,1);
			lcdprint("PRESS        ENT/CNL");
			double_height = 1;
			get_dig_esc_flag = 0;
			tempReceiptNo = get_dig(4, 7, 5, mc.mc.rcpt_no, 1, 0);

			if((!get_dig_esc_flag) && (tempReceiptNo > 0) && (tempReceiptNo < MAX_RCPTNO))
			{
				clrscr();
				gotoxy(3,1);
				lcdprint(" SAVING BILL NUMBER ");
				double_height = 0;
				gotoxy(6,1);
				lcdprint("   PLEASE WAIT...   ");
				mc.mc.rcpt_no = tempReceiptNo;
				store_mc();
			}

			clearProductSummaryForSelectedDay(mc.mc.currentPG);

			load256ProductSummaryinTempArray(1);
			loadProductsStockTempArray(1);
			loadTaxSummaryinTempArray(0, mc.mc.sgst_summ_count, SGST, mc.mc.currentPG);
			loadTaxSummaryinTempArray(0, mc.mc.cgst_summ_count, CGST, mc.mc.currentPG);
			loadTaxSummaryinTempArray(0, mc.mc.igst_summ_count, IGST, mc.mc.currentPG);
			if(mc.mc.gsm_en_dis)
				sendEraseStatus(1, voidRecs, voidTotalAmount);
		}
	}
}
//-------------------------------------------------------------------------
void setReceiptNumberFunction(void)
{
	UINT32 tempReceiptNo = 0;

	clrscr();
	double_height = 1;
	gotoxy(1,1);
	lcdprint("   SET BILL NUMBER  ");
	double_height = 0;
	gotoxy(8,1);
	lcdprint("PRESS        ENT/CNL");
	double_height = 1;
	get_dig_esc_flag = 0;
	tempReceiptNo = get_dig(4, 7, 5, mc.mc.rcpt_no, 1, 0);

	if((!get_dig_esc_flag) && (tempReceiptNo > 0) && (tempReceiptNo < MAX_RCPTNO))
	{
		if(tempReceiptNo > mc.mc.rcpt_no)
		{
			clrscr();
			double_height = 1;
			gotoxy(3,1);
			lcdprint(" SAVING BILL NUMBER ");
			double_height = 0;
			gotoxy(6,1);
			lcdprint("   PLEASE WAIT...   ");
			mc.mc.rcpt_no = tempReceiptNo;
			if((mc.mc.billKotseries == DIFFERENT_SERIES) && (mc.mc.kot_en_dis) && (mc.mc.udf_en_dis))
				 mc.mc.actualRcptNo = tempReceiptNo;
			store_mc();
		}
		else
		{
			clrscr();
			double_height = 1;
			gotoxy(3,1);
			lcdprint(" ENTERED BILL NUMBER");
			gotoxy(5,1);
			lcdprint("  SHOULD BE GREATER ");
			gotoxy(7,1);
			lcdprint(" THAN CURRENT SERIAL");
			beep(ERRRBEEP);
			Delay_ms(2000);
		}
	}
}
//-------------------------------------------------------------------------
void checkUpdateGST(unsigned int sgstRate, unsigned long sgstAmount, unsigned int cgstRate, unsigned long cgstAmount, unsigned int igstRate, unsigned long igstAmount, unsigned char addFlag)
{
	unsigned int loopCount = 0;

	if(sgstAmount > 0)
	{
		for(loopCount = 0; loopCount < mc.mc.sgst_summ_count; loopCount++)
		{
			get_sgst_summ(loopCount, mc.mc.currentPG);
			if(sgst_summary.sgst_summary.sgstRate == sgstRate)
			{
				if(addFlag)
				{
					sgst_summary.sgst_summary.total_amount += sgstAmount;
				}
				else
				{
					if(sgst_summary.sgst_summary.total_amount >= sgstAmount)
						sgst_summary.sgst_summary.total_amount -= sgstAmount;
					else
						sgst_summary.sgst_summary.total_amount = 0;
				}
				store_sgst_summ(loopCount, mc.mc.currentPG);
				break;
			}
		}
	}

	if(cgstAmount > 0)
	{
		for(loopCount = 0; loopCount < mc.mc.cgst_summ_count; loopCount++)
		{
			get_cgst_summ(loopCount, mc.mc.currentPG);
			if(cgst_summary.cgst_summary.cgstRate == cgstRate)
			{
				if(addFlag)
					cgst_summary.cgst_summary.total_amount += cgstAmount;
				else
				{
					if(cgst_summary.cgst_summary.total_amount >= cgstAmount)
						cgst_summary.cgst_summary.total_amount -= cgstAmount;
					else
						cgst_summary.cgst_summary.total_amount = 0;
				}
				store_cgst_summ(loopCount, mc.mc.currentPG);
				break;
			}
		}
	}

	if(igstAmount > 0)
	{
		for(loopCount = 0; loopCount < mc.mc.igst_summ_count; loopCount++)
		{
			get_igst_summ(loopCount, mc.mc.currentPG);
			if(igst_summary.igst_summary.igstRate == igstRate)
			{
				if(addFlag)
					igst_summary.igst_summary.total_amount += igstAmount;
				else
				{
					if(igst_summary.igst_summary.total_amount >= igstAmount)
						igst_summary.igst_summary.total_amount -= igstAmount;
					else
						igst_summary.igst_summary.total_amount = 0;
				}
				store_igst_summ(loopCount, mc.mc.currentPG);
				break;
			}
		}
	}
}
//-------------------------------------------------------------------------
unsigned long long round_off_func(unsigned char type)
{
	UINT64 before_roundoff = 0, temp_amount = 0, temp_amount1 = 0, temp_amount2 = 0;

	before_roundoff = pre_roundoff;

//	hex_asc(before_roundoff, asc, 8);
	value_hex_asc_64U = rec.rec.total_amt;
	hex_asc_64U(asc, 12);

//	temp_amount = asc_hex(asc, 6);
	temp_amount = asc_hex64U(asc, 10);

	temp_amount1 = (temp_amount * 100);

	temp_amount2 = (before_roundoff - temp_amount1);

	switch(type)
	{
		case ROUND_OFF_50P:
								if(temp_amount2 != 0)
								{
									if(temp_amount2 >= 50)
									{
										temp_amount = (100 - temp_amount2);
										return(UINT64)(before_roundoff + temp_amount);
									}
									else if((temp_amount2 < 50) && (temp_amount > 0))
									{
										return(UINT64)(temp_amount1);
									}
									else
										return(UINT64)(before_roundoff);
								}
								else
									return(UINT64)(before_roundoff);
								break;
		case  ROUND_OFF_1R:
								if(temp_amount2 != 0)
								{
									if(temp_amount2 > 50)
									{
										temp_amount = (100 - temp_amount2);
										return(UINT64)(before_roundoff + temp_amount);
									}
									else if((temp_amount2 <= 50) && (temp_amount > 0))
									{
										temp_amount = (50 - temp_amount2);
										return(UINT64)(before_roundoff + temp_amount + 50);
									}
								}
								else
									return(UINT64)(before_roundoff);
								break;
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char count_Agent(void)
{
	BIT exit;
	UCHAR agnt = 0;

	clrscr();
	double_height = 1;
	double_width = 0;

	gotoxy(1,1);
	lcdprint(" PROCESSING  OPR. ");
	gotoxy(4,1);
	lcdprint(" COUNT : ");
	exit = 0;
	agnt = 1;
	while(!exit)
	{
		FlashReadData(((UINT32)AGENT_PG * PGBYTS) + AGENT_START + ((UINT32)agnt * AGENT_TOTAL_LEN), membuf,AGENT_TOTAL_LEN);
		if(membuf[0]== END_CHR)
			exit =1;
		else
			agnt++;

		gotoxy(4,11);
		hex_asc(agnt, asc, 3);
		disp_char(asc[0]);
		disp_char(asc[1]);
		disp_char(asc[2]);
		CheckOnOffKey();
	}
	if(agnt - 1 > MAX_AGENTS)
	{
		clrscr();
		double_height = 1;
		gotoxy(2,1);
		lcdprint("  MAX AGENTS LIMIT  ");
		gotoxy(4,1);
		lcdprint("  PLS REDUCE AGENT  ");
		gotoxy(6,1);
		lcdprint("  AND UPLOAD AGAIN  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		membuf[0] = END_CHR;
		FlashModifyData(((UINT32)AGENT_PG * PGBYTS), membuf, 1);
		return 0;
	}
	if(agnt < 2)
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" INVALID FILE PLEASE");
		gotoxy(5,1);
		lcdprint("  AND UPLOAD AGAIN  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		membuf[0] = END_CHR;
		FlashModifyData(((UINT32)AGENT_PG * PGBYTS), membuf, 1);
		return 0;
	}
	else
		return	agnt -1;
}
//-----------------------------------------------------------------
unsigned int count_prod(void)
{
	BIT exit, i = 0, j = 0;
	UINT16 prd = 0, sgstTemp = 0, cgstTemp = 0;

	clrscr();
	double_height = 1;
	double_width = 0;

	gotoxy(1,1);
	lcdprint(" PROCESSING  PROD. ");
	gotoxy(4,1);
	lcdprint(" COUNT : ");

	exit = 0;
	prd = 1;
	while(!exit)
	{
		FlashReadData(((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)prd * PRODUCT_LEN), membuf,PRODUCT_LEN);
		if(membuf[0]== END_CHR)
			exit =1;
		else
		{
			prd++;

			for(i = PRODUCT_SGST, j = 0; i < (PRODUCT_SGST + PRODUCT_SGST_LEN); i++)
			{
				if(membuf[i] != '.')
					asc[j++] = membuf[i];
			}

			sgstTemp = asc_hex(asc, (PRODUCT_SGST_LEN - 1));

			for(i = PRODUCT_CGST, j = 0; i < (PRODUCT_CGST + PRODUCT_CGST_LEN); i++)
			{
				if(membuf[i] != '.')
					asc[j++] = membuf[i];
			}

			cgstTemp = asc_hex(asc, (PRODUCT_CGST_LEN - 1));

			storeUniqueGST(sgstTemp, cgstTemp, (sgstTemp + cgstTemp));

			gotoxy(4,11);
			hex_asc(prd, asc, 4);
			disp_char(asc[0]);
			disp_char(asc[1]);
			disp_char(asc[2]);
			disp_char(asc[3]);
		}
		CheckOnOffKey();
	}
	if(prd - 1 > MAX_PRODUCTS)
	{
		clrscr();
		double_height = 1;
		gotoxy(2,1);
		lcdprint(" MAX PRODUCTS LIMIT ");
		gotoxy(4,1);
		lcdprint(" PLS REDUCE PRODUCT ");
		gotoxy(6,1);
		lcdprint("  AND UPLOAD AGAIN  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		membuf[0] = END_CHR;
		FlashModifyData(((UINT32)PRODUCT_PG * PGBYTS), membuf, 1);
		return 0;
	}

	loadTaxSummaryinTempArray(0, mc.mc.sgst_summ_count, SGST, mc.mc.currentPG);
	loadTaxSummaryinTempArray(0, mc.mc.cgst_summ_count, CGST, mc.mc.currentPG);
	loadTaxSummaryinTempArray(0, mc.mc.igst_summ_count, IGST, mc.mc.currentPG);

	if(prd < 2)
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" INVALID FILE PLEASE");
		gotoxy(5,1);
		lcdprint("  AND UPLOAD AGAIN  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		membuf[0] = END_CHR;
		FlashModifyData(((UINT32)PRODUCT_PG * PGBYTS), membuf, 1);
		return 0;
	}
	else
		return	prd -1;
}
//-----------------------------------------------------------------
void storeUniqueGST(unsigned int sgst, unsigned int cgst, unsigned int igst)
{
	unsigned int loopCount = 0, foundFlag = 0;

	for(loopCount = 0; loopCount < mc.mc.sgst_summ_count; loopCount++)
	{
		get_sgst_summ(loopCount, 1);
		if(sgst_summary.sgst_summary.sgstRate == sgst)
		{
			foundFlag = 1;
			break;
		}
	}

	if(!foundFlag)
	{
		sgst_summary.sgst_summary.sgstRate = sgst;
		sgst_summary.sgst_summary.total_amount = 0;
		store_sgst_summ(mc.mc.sgst_summ_count, 1);
		store_sgst_summ(mc.mc.sgst_summ_count, 0);
		mc.mc.sgst_summ_count++;
	}

	foundFlag = 0;
	for(loopCount = 0; loopCount < mc.mc.cgst_summ_count; loopCount++)
	{
		get_cgst_summ(loopCount, 1);
		if(cgst_summary.cgst_summary.cgstRate == cgst)
		{
			foundFlag = 1;
			break;
		}
	}

	if(!foundFlag)
	{
		cgst_summary.cgst_summary.cgstRate = cgst;
		cgst_summary.cgst_summary.total_amount = 0;
		store_cgst_summ(mc.mc.cgst_summ_count, 1);
		store_cgst_summ(mc.mc.cgst_summ_count, 0);
		mc.mc.cgst_summ_count++;
	}

	foundFlag = 0;
	for(loopCount = 0; loopCount < mc.mc.igst_summ_count; loopCount++)
	{
		get_igst_summ(loopCount, 1);
		if(igst_summary.igst_summary.igstRate == igst)
		{
			foundFlag = 1;
			break;
		}
	}

	if(!foundFlag)
	{
		igst_summary.igst_summary.igstRate = igst;
		igst_summary.igst_summary.total_amount = 0;
		store_igst_summ(mc.mc.igst_summ_count, 1);
		store_igst_summ(mc.mc.igst_summ_count, 0);
		mc.mc.igst_summ_count++;
	}
}
//-----------------------------------------------------------------
unsigned char count_unit(void)
{
	BIT exit;
	UCHAR prd = 0;
	clrscr();
	double_height = 1;
	double_width = 0;

	gotoxy(1,1);
	lcdprint(" PROCESSING  UNIT ");
	gotoxy(4,1);
	lcdprint(" COUNT : ");
	exit = 0;
	prd = 1;
	while(!exit)
	{
		FlashReadData(((UINT32)UNIT_TYPE_PG * PGBYTS) + UNIT_TYPE_SERIAL_NO + ((UINT32)prd * UNIT_TYPE_LEN), membuf,UNIT_TYPE_LEN);
		if(membuf[0]== END_CHR)
			exit =1;
		else
			prd++;
		gotoxy(4,11);
		hex_asc(prd, asc, 3);
		disp_char(asc[0]);
		disp_char(asc[1]);
		disp_char(asc[2]);
		CheckOnOffKey();
	}
	if(prd - 1 > MAX_UNITS)
	{
		clrscr();
		double_height = 1;
		gotoxy(2,1);
		lcdprint("   MAX UNITS LIMIT  ");
		gotoxy(4,1);
		lcdprint("  PLS REDUCE UNITS  ");
		gotoxy(6,1);
		lcdprint("  AND UPLOAD AGAIN  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		membuf[0] = END_CHR;
		FlashModifyData(((UINT32)UNIT_TYPE_PG * PGBYTS), membuf, 1);
		return 0;
	}
	if(prd < 2)
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" INVALID FILE PLEASE");
		gotoxy(5,1);
		lcdprint("  AND UPLOAD AGAIN  ");
		beep(ERRRBEEP);
		Delay_ms(2000);
		membuf[0] = END_CHR;
		FlashModifyData(((UINT32)UNIT_TYPE_PG * PGBYTS), membuf, 1);
		return 0;
	}
	else
		return	prd -1;
}
//-------------------------------------------------------------------------
void load_defaults(void)
{
	unsigned short int i, j = 0;
	unsigned char temp_apn[] = {"airtelgprs.com"};

	clrscr();
	gotoxy(4,1);
	lcdprint("     LOADING .... ");
	double_height=0;
	gotoxy(7,1);
	lcdprint("    Please Wait !");
	double_height=1;

	if(mc.mc.chkbyt!=CHKBYT)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(2,1);
		lcdprint("  ERASING  MACHINE  ");

		double_height = 1;
		double_width = 0;
		gotoxy(5,1);
		lcdprint("   PLEASE WAIT...   ");

		ERASE_CHIP();

		double_height = 1;
		double_width = 0;
		gotoxy(5,1);
		lcdprint("        DONE        ");

		beep(SHRTBEEP);

		clrscr();
		gotoxy(4,1);
		lcdprint("     LOADING .... ");
		double_height=0;
		gotoxy(7,1);
		lcdprint("    Please Wait !");
		double_height=1;

		mc_pos.mc_pos.position = 0;
		store_mc_position();

		membuf[0]=END_CHR;
		FlashModifyData(((UINT32)PRODUCT_PG * PGBYTS), membuf, 1);
		FlashModifyData(((UINT32)AGENT_PG * PGBYTS), membuf, 1);
		FlashModifyData(((UINT32)HEADER_PG * PGBYTS), membuf, 1);
		FlashModifyData(((UINT32)IMAGE_PG * PGBYTS),membuf,1);
		FlashModifyData(((UINT32)CONFIG_PG * PGBYTS),membuf,1);
		FlashModifyData(((UINT32)UNIT_TYPE_PG * PGBYTS),membuf,1);

		for(i = 0 ; i < ETIMNO_LEN ; i++)
			mc.mc.etimno[i] = '0';

		mc.mc.chkbyt=CHKBYT;
		mc.mc.rcpt_no=1;
		mc.mc.kot_no = 1;
		mc.mc.buzzer_en_dis=1;
		mc.mc.no_of_ers=0;
		mc.mc.twoCopyBill	= 0;
		mc.mc.operator_totalcount = 0;
		mc.mc.recs = 1;
		mc.mc.checkStartRecAddress = 0;
		mc.mc.billNoType = 0;
		mc.mc.paymode = 0;
		mc.mc.current_dd = 0;
		mc.mc.set_current_dd = 0;
		mc.mc.set_current_mm = 0;
		mc.mc.set_current_yy = 0;
		mc.mc.retail_bill_opt = 0;
		mc.mc.cash_reception_enable = 0;
		mc.mc.modify_rate_enable = 0;
		mc.mc.current_mm = 0;
		mc.mc.round_off = 0;
		mc.mc.cust_details_rpt = 0;
		mc.mc.bill_format_selection = 0;
		mc.mc.cgst_summ_count = 0;
		mc.mc.sgst_summ_count = 0;
		mc.mc.prod_totalcount = 0;
		mc.mc.rec_start = 0;
		mc.mc.selected = 0;
		mc.mc.sale_amount = 0;
		mc.mc.discount_en = 0;
		mc.mc.discount_type = 0;
		mc.mc.current_hr = 0;
		mc.mc.prnLogo = 0;
		mc.mc.current_yy = 0;
		mc.mc.udf_en_dis = 0;
		mc.mc.max_table = 0;
		mc.mc.default_slab = 1;
		mc.mc.slab_totalcount = 0;
		mc.mc.kot_en_dis = 0;
		mc.mc.coupon_en_dis = 0;
		mc.mc.rec1 = 1;
		mc.mc.calculatorBillNo = 1;
		mc.mc.cust_name_entry = 0;
		mc.mc.stock_manage_en_dis = 0;
		mc.mc.reverse_tax_en_dis = 0;
		mc.mc.rupeeFlag = 1;
		mc.mc.report_save = 0;
		mc.mc.report_clerk = 0;
		mc.mc.currentPG = 0;
		mc.mc.return_bill_en_dis = 0;
		mc.mc.languageSett = 0;
		mc.mc.drawer_en_dis = 0;
		mc.mc.calculator_swap = 1;
		mc.mc.word_amount = 0;
		mc.mc.weighing_scale = 0;
		mc.mc.baudrate = 1;
		mc.mc.massStrg_barcd_USBKbd_sett = MASS_STORAGE_SETTING;
		mc.mc.net_amt_height = DH;
		mc.mc.operator_details_en_dis = 0;
		mc.mc.gprs_en_dis = 0;
		mc.mc.gsm_en_dis = 0;
		mc.mc.gprs_recordsLot = 10;
		mc.mc.gprs_record_sent = 0;
		mc.mc.gsm_record_sent = 0;
		mc.mc.gsm_recordsLot = 10;
		mc.mc.GPRS_rec_start = 1;
		mc.mc.gsm_interval_data = 0;
		mc.mc.gprs_interval_data = 0;
		mc.mc.print_tax_splitup = 1;
		mc.mc.hsn_en_dis = 0;
		mc.mc.checkOffByte = 0;
		mc.mc.lcd_contrast = 86;
		mc.mc.cust_details0 = 1;
		mc.mc.cust_details1 = 1;
		mc.mc.actualRcptNo = 1;
		mc.mc.billKotseries = DIFFERENT_SERIES;
		mc.mc.time_en_dis = 1;
		mc.mc.srno_en_dis = 1;
		mc.mc.bill_no_char = 0;
		mc.mc.font_height = DH;
		mc.mc.checkIntervalFail = 0;
		mc.mc.lock_dd = 0;
		mc.mc.lock_mm = 0;
		mc.mc.lock_yy = 0;
		mc.mc.app_lock = 0;
		mc.mc.app_lock_setting = 0;
		mc.mc.lastMissedCallCount = 0;
		mc.mc.qty_rate_swap = QTY_RATE;
		mc.mc.qty_enter_method = QTY_3_3;
		mc.mc.digitsBeforeDot = 3;
		mc.mc.digitsAfterDot = 3;
		mc.mc.barcode_qty_en_dis = 0;
		mc.mc.product_selection = BEFORE_QTY;
		mc.mc.igstFlag = 0;
		mc.mc.igst_summ_count = 0;
		mc.mc.parcel_en_dis = 0;
		mc.mc.parcel_rate_amt = 0;
		mc.mc.weighing_scale_mode = 1; //vikas
		mc.mc.iw_disc_percent_auto_manual = 0;
		mc.mc.kot_qty_amt_print = 0;
		mc.mc.on_off_bill_wise_sms = 0;
		mc.mc.item_qty_print = 1;		//akshay 051019
		strncpy(mc.mc.billno_field, "BILL NO   ", BILL_NAME_LEN);

		mc.mc.lastTicket = 1;
		mc.mc.stock_minus = 1;
		mc.mc.last_amt_display_en_dis = 1;
		for(i = 0 ; i < APN_STRING_LEN ; i++)
			mc.mc.apn[i] = '\0';

		strcpy(mc.mc.apn,temp_apn);

		mc.mc.baseAmountOrMRP = BaseAmount;		//Mrugnil 18/02/25
		mc.mc.fontStyle = OldFont;		//Mrugnil 18/02/25

		store_mc();

		hex_asc(mc.mc.udf_en_dis, asc,CONFIG_FLAG_LEN);
		FlashModifyData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_FLAG), asc, CONFIG_FLAG_LEN);

		hex_asc(mc.mc.kot_en_dis, asc,CONFIG_FLAG_LEN);
		FlashModifyData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_KOT_FLAG), asc, CONFIG_FLAG_LEN);

		hex_asc(mc.mc.max_table, asc,MAX_TABLES_LEN);
		FlashModifyData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_MAX_TABLES), asc, MAX_TABLES_LEN);

		changeBaudRate(BR115200);

		memset(mob_details.arr, '\0', MOBILE_UNION);
		store_mobile_details();

		ws_sett.ws_sett.end = 6;
		ws_sett.ws_sett.start = 0;
		store_ws_sett();

		for(i = 0; i < DAILY_COLL_LEN; i++)
			daily.arr[i] = 0;

		for(i = 0; i < MONTHLY_COLL_LEN; i++)
			monthly.arr[i] = 0;

		for(i = 0; i < YEARLY_COLL_LEN; i++)
			yearly.arr[i] = 0;

		set_lcd_contrast(mc.mc.lcd_contrast);

		set_dattim(0);
		clrscr();
		gotoxy(4,1);
		lcdprint("     LOADING .... ");
		double_height=0;
		gotoxy(7,1);
		lcdprint("    Please Wait !");
		double_height=1;

	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

		for(i = 0; i < 3; i++)
		{
			FlashModifyData((((unsigned long)(DAILY_COLL_BASE_ADDR * PGBYTS)) + ((unsigned long)(i * 4096))), g_8K_membuf, 4095);
		}

		for(i = 0; i < 9; i++)
		{
			FlashModifyData((((unsigned long)(MONTHLY_COLL_BASE_ADDR * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
		}

		FlashModifyData(((unsigned long)(YEARLY_COLL_BASE_ADDR * PGBYTS)), g_8K_membuf, 255);

		for(i = 0; i < 3; i++)
		{
			FlashModifyData((((unsigned long)(SLAB_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
		}

		for(i = 0; i < 9; i++)
		{
			FlashModifyData((((unsigned long)(UDF_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
		}

		FlashModifyData(((unsigned long)(HOT_KEY_PG * PGBYTS)), g_8K_membuf, 255);

		for(i = 0; i < 6; i++)
		{
			FlashModifyData(((unsigned long)(PRODUCT_SUMMARY_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
			FlashModifyData(((unsigned long)(PRODUCT_SUMMARY1_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
		}

		FlashModifyData(((unsigned long)(PRODUCT_SUMMARY_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 1024);
		FlashModifyData(((unsigned long)(PRODUCT_SUMMARY1_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 1024);

		FlashModifyData(((unsigned long)(SGST_SUMMARY_PG * PGBYTS)), g_8K_membuf, 4095);
		FlashModifyData(((unsigned long)(SGST_SUMMARY1_PG * PGBYTS)), g_8K_membuf, 4095);
		FlashModifyData(((unsigned long)(CGST_SUMMARY_PG * PGBYTS)), g_8K_membuf, 4095);
		FlashModifyData(((unsigned long)(CGST_SUMMARY1_PG * PGBYTS)), g_8K_membuf, 4095);

//		for(i = 0; i < RPT_PAGE_UNION_LEN; i++)		//pending
//			rpt_details.arr[i] = 0;

		memset(g_8K_membuf,0,sizeof(g_8K_membuf));
		FlashModifyData((((unsigned long)(REPORT_DETAILS_COUNT_PG * PGBYTS))), g_8K_membuf, (RPT_PAGE_UNION_LEN * 370));

//		rpt_details_count.rpt_details_count.bwr_count = 0;
//		rpt_details_count.rpt_details_count.iwr_count = 0;
//		for(i = 0; i <= 366; i++)
//			store_rpt_details_count(i);

		rpt_details.rpt_details.bwr_ptr = 1;
		rpt_details.rpt_details.iwr_ptr = 1;
		store_rpt_details();

		memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
		for(i = 0; i < 7; i++)
		{
			FlashModifyData(((unsigned long)(TWR_1_TO_15DAYS * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
		}
		FlashModifyData(((unsigned long)(TWR_1_TO_15DAYS * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 928);

		twr_ptr.twr_ptr.day_ptr = 1;
		store_twr_ptr();

		FlashModifyData((((UINT32)CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME),  "TABLE NO            ", UDF_NAME_LEN);

		//nilesh
		for(i = 0; i < 9; i++) //clearing barcode details
		{
			FlashModifyData(((unsigned long)(BARCODE_DETAILS_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
		}
		FlashModifyData(((unsigned long)(BARCODE_DETAILS_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 3136);

		for(i = 0; i < 2; i++)
		{
			FlashModifyData((((unsigned long)(UDF_PRD_CNT_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
		}

		FlashModifyData(((unsigned long)(UDF_REC_PG1 * PGBYTS)), g_8K_membuf, 255);
		FlashModifyData(((unsigned long)(UDF_REC_PG2 * PGBYTS)), g_8K_membuf, 255);

		store_call_details1(g_8K_membuf);
		store_call_details2(g_8K_membuf);
		store_call_details3(g_8K_membuf);

		FlashModifyData(((UINT32)RATE_SLAB_NAME * PGBYTS),g_8K_membuf,255);

		for(i = 0; i <= MAX_PRODUCTS; i++)
		{
			memset(temp_product_summary[i].arr, 0, PRODUCT_SUMMARY_LEN);
		}

		for(i = 0; i <= MAX_PRODUCTS; i++)
		{
			memset(temp_product_stock[i].arr, 0, PRODUCT_STOCK_UNION_LEN);
		}

		for(i = 0; i < TEMP_TAX_SUMM_ARRAY; i++)
		{
			memset(SGST_tax_summary[i].arr, 0, TAX_SUMMARY_LEN);
			memset(CGST_tax_summary[i].arr, 0, TAX_SUMMARY_LEN);
			memset(IGST_tax_summary[i].arr, 0, TAX_SUMMARY_LEN);
		}
	}
//	store_mc();
	clrscr();
}
//------------------------------------------------------------------------
void turnOffPrinter(void)
{
	if(printerIdle > 5000)
	{
		if(PRN_STROBE_EN == 1)
			prn_pow(PRINTER_OFF);
		printerIdle = 0;
	}
}
//------------------------------------------------------------------------
void checkDateChange(void)
{
	unsigned int count = 0;
	UINT32 delay = 10;

	checkAppLockDate();

	read_rtc();
	tim_hex_asc();

	if(((mc.mc.set_current_dd != tim.tim.dd) || (mc.mc.set_current_mm != tim.tim.mm) || (mc.mc.set_current_yy != tim.tim.yy)) && (!mc.mc.app_lock))
	{
		if(mc.mc.gprs_en_dis)
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3,1);
			lcdprint(" SENDING UNSENT DATA");
			double_height = 0;
			gotoxy(5,1);
			lcdprint("   PLEASE WAIT...   ");
			Delay_ms(2000);
			modemPushTimeOut = 0;
			stopPostCMDToModem();
			pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
			sendAllDataCMDToModem();
		}
		Delay_ms(delay);
		store_daily_summary(mc.mc.current_dd);
		Delay_ms(delay);
		store_monthly_summary(mc.mc.current_mm);
		Delay_ms(delay);
		store_yearly_summary(mc.mc.current_yy);

		Delay_ms(delay);
		updateFlashContents();

		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(3,1);
		lcdprint(" PROCESSING RECORDS ");
		double_height = 0;
		gotoxy(5,1);
		lcdprint("   PLEASE WAIT...   ");

		if(mc.mc.currentPG)
		{
			if(mc.mc.recs > 1)
				mc.mc.currentPG = 0;
		}
		else
		{
			if(mc.mc.rec1 > 1)
				mc.mc.currentPG = 1;
		}

		if(mc.mc.currentPG)
		{
			mc.mc.recs = 1;
		}
		else
		{
			mc.mc.rec1 = 1;
		}

		if(mc.mc.currentPG)
		{
			mc.mc.cust_details0 = 1;
		}
		else
		{
			mc.mc.cust_details1 = 1;
		}

		if(mc.mc.billNoType)
		{
			mc.mc.rcpt_no = 1;
			mc.mc.kot_no = 1;
			mc.mc.actualRcptNo = 1;
			mc.mc.calculatorBillNo = 1;
		}

		mc.mc.GPRS_rec_start = 1;
		mc.mc.lastTicket = 1;

		Delay_ms(delay);
		if(mc_pos.mc_pos.position < 5)
		{
			mc_pos.mc_pos.position++;
			store_mc_position();
		}
		else
		{
			mc_pos.mc_pos.position = 0;
			store_mc_position();
		}
		Delay_ms(delay);
		mc.mc.checkStartRecAddress = 0;
		store_mc();
	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
		Delay_ms(delay);
		if(mc.mc.currentPG)
			FlashModifyData(((unsigned long)(UDF_REC_PG1 * PGBYTS)), g_8K_membuf, 255);
		else
			FlashModifyData(((unsigned long)(UDF_REC_PG2 * PGBYTS)), g_8K_membuf, 255);

		rpt_details.rpt_details.bwr_ptr++;
		if(rpt_details.rpt_details.bwr_ptr > 366)
			rpt_details.rpt_details.bwr_ptr = 1;

		rpt_details.rpt_details.iwr_ptr++;
		if(rpt_details.rpt_details.iwr_ptr > 366)
			rpt_details.rpt_details.iwr_ptr = 1;

		Delay_ms(delay);
		store_rpt_details();

		rpt_details_count.rpt_details_count.bwr_count = 0;
		rpt_details_count.rpt_details_count.iwr_count = 0;
		store_rpt_details_count(rpt_details.rpt_details.bwr_ptr);

		twr_ptr.twr_ptr.day_ptr++;
		if(twr_ptr.twr_ptr.day_ptr > 366)
			twr_ptr.twr_ptr.day_ptr = 1;

		Delay_ms(delay);
		store_twr_ptr();

		for(count = 0; count < TWR_UNION_LEN; count++)
			twr.arr[count] = 0;
		store_twr(twr_ptr.twr_ptr.day_ptr);

		if(mc.mc.udf_en_dis)
			shiftOpenKOTtoCurrentDay();

		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(3,1);
		lcdprint(" PROCESSING RECORDS ");
		double_height = 0;
		gotoxy(5,1);
		lcdprint("   PLEASE WAIT...   ");

		Delay_ms(delay);
		for(count = 0; count < mc.mc.sgst_summ_count; count++)
		{
			get_sgst_summ(count, mc.mc.currentPG);
			sgst_summary.sgst_summary.total_amount = 0;
			store_sgst_summ(count, mc.mc.currentPG);
		}
		Delay_ms(delay);
		for(count = 0; count < mc.mc.cgst_summ_count; count++)
		{
			get_cgst_summ(count, mc.mc.currentPG);
			cgst_summary.cgst_summary.total_amount = 0;
			store_cgst_summ(count, mc.mc.currentPG);
		}
		Delay_ms(delay);
		for(count = 0; count < mc.mc.igst_summ_count; count++)
		{
			get_igst_summ(count, mc.mc.currentPG);
			igst_summary.igst_summary.total_amount = 0;
			store_igst_summ(count, mc.mc.currentPG);
		}
		Delay_ms(delay);
		clearProductSummaryForSelectedDay(mc.mc.currentPG);

		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(3,1);
		lcdprint(" PROCESSING RECORDS ");
		double_height = 0;
		gotoxy(5,1);
		lcdprint("   PLEASE WAIT...   ");

		mc.mc.set_current_dd = tim.tim.dd;
		mc.mc.current_dd++;
		if(mc.mc.current_dd > 365)
			mc.mc.current_dd = 0;
		store_mc();

		get_daily_summary(mc.mc.current_dd);
		daily.daily.dayNo = asc_hex(dd2, 2);
		daily.daily.monthNo = asc_hex(mm2, 2);
		daily.daily.yearNo = asc_hex(yy2, 2);
		daily.daily.dailyBillNo = 0;
		daily.daily.dailyTotAmnt = 0;
		daily.daily.dailyTotDisc = 0;
		daily.daily.dailyTotTax = 0;
		store_daily_summary(mc.mc.current_dd);

		if(mc.mc.set_current_mm != tim.tim.mm)
		{
			mc.mc.set_current_mm = tim.tim.mm;
			mc.mc.current_mm++;
			if(mc.mc.current_mm > 60)
				mc.mc.current_mm = 0;
			store_mc();

			get_monthly_summary(mc.mc.current_mm);
			monthly.monthly.monthNo = asc_hex(mm2, 2);
			monthly.monthly.yearNo = asc_hex(yy2, 2);
			monthly.monthly.monthlyBillNo = 0;
			monthly.monthly.monthlyTotAmnt = 0;
			monthly.monthly.monthlyTotDisc = 0;
			monthly.monthly.monthlyTotTax = 0;
			store_monthly_summary(mc.mc.current_mm);
		}

		if(mc.mc.set_current_yy != tim.tim.yy)
		{
			mc.mc.set_current_yy = tim.tim.yy;
			mc.mc.current_yy++;
			if(mc.mc.current_yy > 5)
				mc.mc.current_yy = 0;
			store_mc();

			get_yearly_summary(mc.mc.current_yy);
			yearly.yearly.yearNo = asc_hex(yy2, 2);
			yearly.yearly.yearlyBillNo = 0;
			yearly.yearly.yearlyTotAmnt = 0;
			yearly.yearly.yearlyTotDisc = 0;
			yearly.yearly.yearlyTotTax = 0;
			store_yearly_summary(mc.mc.current_yy);
		}

		load256ProductSummaryinTempArray(1);
		loadProductsStockTempArray(1);
		loadTaxSummaryinTempArray(0, mc.mc.sgst_summ_count, SGST, mc.mc.currentPG);
		loadTaxSummaryinTempArray(0, mc.mc.cgst_summ_count, CGST, mc.mc.currentPG);
		loadTaxSummaryinTempArray(0, mc.mc.igst_summ_count, IGST, mc.mc.currentPG);

		store_mc();

		if(mc.mc.gprs_en_dis || mc.mc.gsm_en_dis)
		{
//			set_rtc_from_quectel();//vikas
			if(mc.mc.set_current_dd != tim.tim.dd)
			{
				mc.mc.set_current_dd = tim.tim.dd;
				mc.mc.current_dd++;
				if(mc.mc.current_dd > 365)
					mc.mc.current_dd = 0;
			}

			if(mc.mc.set_current_mm != tim.tim.mm)
			{
				mc.mc.set_current_mm = tim.tim.mm;
				mc.mc.current_mm++;
				if(mc.mc.current_mm > 60)
					mc.mc.current_mm = 0;
			}

			if(mc.mc.set_current_yy != tim.tim.yy)
			{
				mc.mc.set_current_yy = tim.tim.yy;
				mc.mc.current_yy++;
				if(mc.mc.current_yy > 5)
					mc.mc.current_yy = 0;
			}
			store_mc();
		}
		double_height = 0;
		gotoxy(5,1);
		lcdprint("        DONE        ");
		beep(SHRTBEEP);
		clrscr();
	}

	if(mc.mc.current_hr == 0)
	{
		mc.mc.current_hr = tim.tim.hr;
		mc.mc.gsm_interval_data = 0;
		mc.mc.gprs_interval_data = 0;
		store_mc();
	}
	else if(mc.mc.current_hr != tim.tim.hr)
	{
		mc.mc.current_hr = tim.tim.hr;
		mc.mc.gsm_interval_data = 1;
		mc.mc.gprs_interval_data = 1;
		mc.mc.checkIntervalFail = 0;
		store_mc();
	}
}
//------------------------------------------------------------------------
void shiftOpenKOTtoCurrentDay(void)
{
	UINT32 endRecs = 0, loop_count = 0, pageSelect = 0, startRecs = 1, dataCount = 0, recCountLoop = 0;
	UCHAR i = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(3,1);
	lcdprint(" PROCESSING RECORDS ");
	double_height = 0;
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT...   ");
	if(mc.mc.currentPG)
	{
		endRecs = mc.mc.rec1;
		pageSelect = 0;
	}
	else
	{
		endRecs = mc.mc.recs;
		pageSelect = 1;
	}

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

	for(i = 0; i < 9; i++)
	{
		FlashModifyData((((unsigned long)(UDF_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
	}

	for(loop_count = 1, startRecs = 1; loop_count < endRecs; loop_count++)
	{
		get_rec_report(loop_count, pageSelect);

		if(rec.rec.bill_stat == 'O')
		{

			get_udf(rec.rec.udf_no);
			if(!udfno.udfno.udfActive)
			{
				udfno.udfno.udfActive = 1;
				udfno.udfno.udfStart = startRecs;
				udfno.udfno.udfRcptNo = mc.mc.rcpt_no++;
				store_udf(rec.rec.udf_no);
			}

			rec.rec.rcpt_no = udfno.udfno.udfRcptNo;

			for(i = 0; i < RECDAT_LEN; i++)
				g_8K_membuf[dataCount++] = rec.arr[i];
			recCountLoop++;
		}

		if(dataCount > TEMP_REC_PUSH_ARRAY)
		{
			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(startRecs * RECDAT_LEN))), g_8K_membuf, dataCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(startRecs * RECDAT_LEN))), g_8K_membuf, dataCount);
			startRecs += recCountLoop;
			recCountLoop = 0;
			dataCount = 0;
		}
		else if((dataCount <= TEMP_REC_PUSH_ARRAY) && (loop_count == (endRecs - 1)))
		{
			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(startRecs * RECDAT_LEN))), g_8K_membuf, dataCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(startRecs * RECDAT_LEN))), g_8K_membuf, dataCount);
			startRecs += recCountLoop;
			recCountLoop = 0;
			dataCount = 0;
		}
	}

	if(mc.mc.currentPG)
		mc.mc.recs = startRecs;
	else
		mc.mc.rec1 = startRecs;
	store_mc();
}
//------------------------------------------------------------------------
void erase_recs(void)
{
	BIT entry = 0;
	UINT32 loop = 0, count = 0;
	UINT64 amount = 0;
	entry = 1;
	if(entry)
	{
		clrscr();
		beep(LONGBEEP);
		gotoxy(4,1);
		lcdprint(" ERASING RECORDS... ");
		double_height = 0;
		gotoxy(7,1);
		lcdprint("    Please Wait !   ");
		double_height = 1;

		count = 0;
		amount = 0;

		for(loop = 1; loop < mc.mc.recs; loop++)
		{
			get_rec_report(loop, 1);
			if(rec.rec.last == 1)
			{
				if(rec.rec.bill_stat == 'C')
				{
					if((rec.rec.calc_stat == 0) || (rec.rec.calc_stat == 'C'))
					{
						count++;
						amount += rec.rec.round_off_tot_amt;
					}
					else if((rec.rec.calc_stat == 'R') && (rec.rec.returnedProduct != 1))
					{
						if(count > 0)
							count--;
						else
							count = 0;
						if(amount >= rec.rec.round_off_tot_amt)
							amount -= rec.rec.round_off_tot_amt;
						else
							amount = 0;
					}
				}
			}
		}

		for(loop = 1; loop < mc.mc.rec1; loop++)
		{
			get_rec_report(loop, 0);
			if(rec.rec.last == 1)
			{
				if(rec.rec.bill_stat == 'C')
				{
					if((rec.rec.calc_stat == 0) || (rec.rec.calc_stat == 'C'))
					{
						count++;
						amount += rec.rec.round_off_tot_amt;
					}
					else if((rec.rec.calc_stat == 'R') && (rec.rec.returnedProduct != 1))
					{
						if(count > 0)
							count--;
						else
							count = 0;
						if(amount >= rec.rec.round_off_tot_amt)
							amount -= rec.rec.round_off_tot_amt;
						else
							amount = 0;
					}
				}
			}
		}
		read_rtc();
		ers.ers.hr = tim.tim.hr;
		ers.ers.mn = tim.tim.mn;
		ers.ers.sc = tim.tim.sc;
		ers.ers.dd = tim.tim.dd;
		ers.ers.mm = tim.tim.mm;
		ers.ers.yy = tim.tim.yy;
		ers.ers.tickets = count;
		ers.ers.mac_totalamount = amount;
		ers.ers.stat = '#';
		store_erase_summary(mc.mc.no_of_ers);
		mc.mc.no_of_ers++;
		mc.mc.recs = 1;
		mc.mc.rec1 = 1;
		mc.mc.checkStartRecAddress = 0;
		mc.mc.rec_start = 0;
		mc.mc.selected = 0;
		mc.mc.sale_amount = 0;
		mc.mc.rcpt_no = 1;
		if(mc.mc.no_of_ers >= ERS_REC)
			mc.mc.no_of_ers = 0;

		mc.mc.GPRS_rec_start = 1;
		mc.mc.cust_details0 = 1;
		mc.mc.cust_details1 = 1;

		store_mc();

		clrscr();
		double_width = 0;
		double_height = 1;
		gotoxy(2,3);
		lcdprint("    ALL RECORDS");
		gotoxy(5,8);
		lcdprint("ERASED");
		beep(OKKKBEEP);
		Delay_ms(3500);
		voidTotalAmount = ers.ers.mac_totalamount;
		voidRecs = ers.ers.tickets;
	}
}
//-------------------------------------------------------------------------
void saveSummaryDetails(void)
{
//	read_rtc();
//	tim_hex_asc();

	pk_unpk(rec.rec.dd, dd2);
	pk_unpk(rec.rec.mm, mm2);
	pk_unpk(rec.rec.yy, yy2);
	pk_unpk(rec.rec.hr, hr2);
	pk_unpk(rec.rec.mn, mn2);
	pk_unpk(rec.rec.sc, sc2);

//	get_daily_summary(mc.mc.current_dd);
	daily.daily.daily_set_var = 'Y';
	daily.daily.dayNo = asc_hex(dd2, 2);
	daily.daily.monthNo = asc_hex(mm2, 2);
	daily.daily.yearNo = asc_hex(yy2, 2);
	if(!returnBill)
	{
		daily.daily.dailyBillNo++;
		daily.daily.dailyTotAmnt += rec.rec.round_off_tot_amt;
		daily.daily.dailyTotDisc += rec.rec.discount_amt;
		daily.daily.dailyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
	}
	else
	{
		daily.daily.dailyBillNo++;
		if(daily.daily.dailyTotAmnt >= rec.rec.round_off_tot_amt)
			daily.daily.dailyTotAmnt -= rec.rec.round_off_tot_amt;
		else
			daily.daily.dailyTotAmnt = 0;
		if(daily.daily.dailyTotDisc >= rec.rec.discount_amt)
			daily.daily.dailyTotDisc -= rec.rec.discount_amt;
		else
			daily.daily.dailyTotDisc = 0;
		if(daily.daily.dailyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
			daily.daily.dailyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
		else
			daily.daily.dailyTotTax = 0;
	}
//	store_daily_summary(mc.mc.current_dd);

//	get_monthly_summary(mc.mc.current_mm);
	monthly.monthly.monthNo = asc_hex(mm2, 2);
	monthly.monthly.yearNo = asc_hex(yy2, 2);
	monthly.monthly.monthly_set_var = 'Y';
	if(!returnBill)
	{
		monthly.monthly.monthlyBillNo++;
		monthly.monthly.monthlyTotAmnt += rec.rec.round_off_tot_amt;
		monthly.monthly.monthlyTotDisc += rec.rec.discount_amt;
		monthly.monthly.monthlyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
	}
	else
	{
		monthly.monthly.monthlyBillNo++;
		if(monthly.monthly.monthlyTotAmnt >= rec.rec.round_off_tot_amt)
			monthly.monthly.monthlyTotAmnt -= rec.rec.round_off_tot_amt;
		else
			monthly.monthly.monthlyTotAmnt = 0;
		if(monthly.monthly.monthlyTotDisc >= rec.rec.discount_amt)
			monthly.monthly.monthlyTotDisc -= rec.rec.discount_amt;
		else
			monthly.monthly.monthlyTotDisc = 0;
		if(monthly.monthly.monthlyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
			monthly.monthly.monthlyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
		else
			monthly.monthly.monthlyTotTax = 0;
	}
//	store_monthly_summary(mc.mc.current_mm);

//	get_yearly_summary(mc.mc.current_yy);
	yearly.yearly.yearly_set_var = 'Y';
	yearly.yearly.yearNo = asc_hex(yy2, 2);
	yearly.yearly.yearly_set_var = 'Y';
	if(!returnBill)
	{
		yearly.yearly.yearlyBillNo++;
		yearly.yearly.yearlyTotAmnt += rec.rec.round_off_tot_amt;
		yearly.yearly.yearlyTotDisc += rec.rec.discount_amt;
		yearly.yearly.yearlyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
	}
	else
	{
		yearly.yearly.yearlyBillNo++;
		if(yearly.yearly.yearlyTotAmnt >= rec.rec.round_off_tot_amt)
			yearly.yearly.yearlyTotAmnt -= rec.rec.round_off_tot_amt;
		else
			yearly.yearly.yearlyTotAmnt = 0;
		if(yearly.yearly.yearlyTotDisc >= rec.rec.discount_amt)
			yearly.yearly.yearlyTotDisc -= rec.rec.discount_amt;
		else
			yearly.yearly.yearlyTotDisc = 0;
		if(yearly.yearly.yearlyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
			yearly.yearly.yearlyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
		else
			yearly.yearly.yearlyTotTax = 0;
	}
//	store_yearly_summary(mc.mc.current_yy);
}
//-------------------------------------------------------------------------
void restore_mc(void)
{
	unsigned short int pgg_no;
	unsigned char exit=0, i=0, buff[3];
	unsigned char* nopgmsg[6]=
	{
		" NO HEADER DATA!      ",
		" NO OPERATOR DATA!    ",
		" NO PRODUCT DATA!     ",
		" NO UNIT TYPE DATA!   ",
		" NO CONFIG DATA!      ",
		" NO PRINT LOGO DATA!  "
	};
	UINT32 recs = 0;

	get_mc();
	if(mc.mc.chkbyt!=CHKBYT)
	{
		Delay_ms(500);
		get_mc();
		if(mc.mc.chkbyt!=CHKBYT)
		{
			Delay_ms(500);
			get_backup();
			if(mc.mc.chkbyt!=CHKBYT)
			{
				Delay_ms(500);
				get_backup();
				if(mc.mc.chkbyt!=CHKBYT)
				{
					clrscr();
					gotoxy(3,1);
					double_height = 1;
					lcdprint("   LOAD MASTER DATA!");
					double_height=0;
					gotoxy(5,1);
					lcdprint("   PRESS ANY KEY ...");
					double_height=1;
					while(!keyPressCheck());
					if(pwd_entry(MASTER_PASSWORD))
					{
						load_defaults();
					}
					else
					{
						clrscr();
						gotoxy(3,1);
						lcdprint(" PLEASE RESTART M/C ");
						while(1)
						{
							while(!(keyPressCheck()));
						}
					}
				}
				else
				{
					restore_backup();
				}
			}
			else
			{
				restore_backup();
			}
		}
	}

	get_mc();

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	get_rpt_details();
	get_rpt_details_count(rpt_details.rpt_details.bwr_ptr);
	get_twr_ptr();
	get_twr(twr_ptr.twr_ptr.day_ptr);

	if(recs > 1)
	{
		checkFailedSwitchOff();
	}

	get_daily_summary(mc.mc.current_dd);
	get_monthly_summary(mc.mc.current_mm);
	get_yearly_summary(mc.mc.current_yy);

	clrscr();
	exit=0;
	i=0;

	for(i = 0, exit = 0; i < 6; i++)
	{
		buff[0] = ' ';
		switch(i)
		{
			case 0:
				pgg_no = HEADER_PG;
				break;
			case 1:
				pgg_no = AGENT_PG;
				break;
			case 2:
				pgg_no = PRODUCT_PG;
				break;
			case 3:
				pgg_no = UNIT_TYPE_PG;
				break;
			case 4:
				pgg_no = CONFIG_PG;
				break;
			case 5:
				pgg_no = IMAGE_PG;
				break;
		}
		FlashReadData(((unsigned long)pgg_no*PGBYTS),buff,1);
		if(buff[0]=='$')
		{
			exit++;
		}
	}

	if(exit < 6)
	{
		if(optionHMSCorUART())
		{
			UpdateAllMastersFromMSC(1);	//Yogesh190517
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(3,1);
			lcdprint("   PLEASE RESTART   ");
			gotoxy(5,1);
			lcdprint("       MACHINE      ");
			while(1)
			{
				while(!(keyPressCheck()));
			}
		}
	}

	clrscr();
	exit = 0;
	i = 0;
	psp_usb_otg_set_id(PSP_USB_OTG_ID_DEVICE);
	while(!exit)
	{
		USB_CDC_Scheduler();
		chk_comm();
		blon();
		gotoxy(3,1);
		switch(i)
		{
			case 0:
					pgg_no = HEADER_PG;
					break;
			case 1:
					pgg_no = AGENT_PG;
					break;
			case 2:
				  	pgg_no = PRODUCT_PG;
					break;
			case 3:
				  	pgg_no = UNIT_TYPE_PG;
					break;
			case 4:
					pgg_no = CONFIG_PG;
					break;
			case 5:
					pgg_no = IMAGE_PG;
					break;
		}
		FlashReadData(((unsigned long)pgg_no*PGBYTS),buff,1);
		if(buff[0]=='$')
		{
			if(i < 6)
				i++;
			else
				exit = 1;
		}
		else
		{
			double_height = 1;
			get_mc();
			lcdprint(nopgmsg[i]);
		}
		double_height = 0;
		CheckOnOffKey();
	}
	psp_usb_otg_set_id(PSP_USB_OTG_ID_HOST);

	read_rtc();

	if(mc.mc.udf_en_dis)
	{
		FlashReadData((((UINT32) CONFIG_PG * PGBYTS) + CONFIG_UDF_NAME), asc, UDF_NAME_LEN);
	}

	loadConfigDetails();

	if(mc.mc.udf_en_dis)
		udf_no = 0;
	else
		udf_no = 1;

	if(mc.mc.udf_en_dis)
		mc.mc.selected = 0;

	read_rtc();

	if(mc.mc.billNoType)
	{
		if(mc.mc.set_current_dd != tim.tim.dd)
		{
			mc.mc.rcpt_no = 1;
			mc.mc.calculatorBillNo = 1;
			store_mc();
		}
	}

	if(mc.mc.udf_en_dis)
		mc.mc.return_bill_en_dis = 0;
//	if(mc.mc.twoCopyBill)		//Mrugnil 30/01/24
//		mc.mc.coupon_en_dis = 0;

	store_mc();
}
//-------------------------------------------------------------------------
unsigned char optionHMSCorUART(void)
{
	UCHAR key = 0, exit = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(1,1);
	lcdprint("   LOAD DATA FROM   ");
	gotoxy(3,1);
	lcdprint("1. USB PENDRIVE     ");
	gotoxy(5,1);
	lcdprint("2. PC SERIAL PORT   ");

	double_height = 0;
	gotoxy(8,1);
	lcdprint("    PRESS CHOICE    ");

	while(!exit)
	{
		key = 0;

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case '1':
						return 1;
			case '2':
						return 0;
		}
	}

	return 0;
}
//-------------------------------------------------------------------------
unsigned int productEnteryEdit(unsigned int entryIndex)
{
	UINT32 loopCount, storeCount, prodPosition = 0, scrollStart = 0, tempPos = 0;
	UINT8 key;
	BIT exit = 0, shiftFlag = 0, found = 0, scrollFlag = 0, i = 0;
	unsigned int constPrd = 0;

	memset(tempArr, 0, 6);
	hex_asc(entryIndex, tempArr, 4);
	countSI = 0;

	for(i = 0; i < 4; i++)
	{
		if(tempArr[i] != ' ')
			countSI++;
	}

	constPrd = entryIndex;

	scrollStart = 1;
	while(!exit)
	{
		found = 0;

		if(scrollFlag)
		{
			prodPosition = scrollStart;
			FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);
			if(membuf[PRODUCT_EN_FLAG] != 1)
				found = 1;
			else
				found = 0;
		}
		else
		{
			prodPosition = productBinarySearch(entryIndex);
			if(prodPosition > 0)
			{
				found = 1;
				scrollStart = prodPosition;
			}
			else
				found = 0;
		}

		if(found)
		{
			FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)prodPosition * PRODUCT_LEN), membuf, PRODUCT_LEN);

			clrscr();
			double_height = 0;
			double_width = 1;
			gotoxy(1,1);
			lcdprint("   ITEMS  ");

			double_height = 1;
			double_width = 0;
			gotoxy(2,1);
			for(loopCount = 0; loopCount < PRODUCT_SER_NO_LEN; loopCount++)
			{
				disp_char(membuf[PRODUCT_SER_NO + loopCount]);
			}
			gotoxy(4,1);
			//print the product Name
//			for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN - 7; loopCount++)
//			{
//				disp_char(membuf[PRODUCT_TEXT + loopCount]);
//			}

			for(loopCount = 0; loopCount < PRODUCT_TEXT_LEN; loopCount++)		//Mrugnil 10/01/25
			{
				disp_char(membuf[PRODUCT_TEXT + loopCount]);		//Mrugnil 10/01/25
			}		//Mrugnil 10/01/25

			double_height = 1;
			double_width = 0;
			gotoxy(6,1);

			switch(slab_no)
			{
				case 1:
						for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT_LEN; loopCount++)
						{
							if(membuf[PRODUCT_AMOUNT + loopCount] != '.')
							{
								asc[storeCount++] = membuf[PRODUCT_AMOUNT + loopCount];
							}
						}
						temprec.temprec.rate = asc_hex(asc, PRODUCT_AMOUNT_LEN - 1);
						break;
				case 2:
						for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT2_LEN; loopCount++)
						{
							if(membuf[PRODUCT_AMOUNT2 + loopCount] != '.')
							{
								asc[storeCount++] = membuf[PRODUCT_AMOUNT2 + loopCount];
							}
						}
						temprec.temprec.rate = asc_hex(asc, PRODUCT_AMOUNT2_LEN - 1);
						break;
				case 0:
				case 3:
						for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT3_LEN; loopCount++)
						{
							if(membuf[PRODUCT_AMOUNT3 + loopCount] != '.')
							{
								asc[storeCount++] = membuf[PRODUCT_AMOUNT3 + loopCount];
							}
						}
						temprec.temprec.rate = asc_hex(asc, PRODUCT_AMOUNT3_LEN - 1);
						break;

			}

			rem_zeros(asc, 5);
			for(loopCount = 0; loopCount < (PRODUCT_AMOUNT_LEN - 1); loopCount++)
			{
				if(loopCount == 6)
					disp_char('.');
				disp_char(asc[loopCount]);
			}

			//get product discount
			for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_DISCOUNT_LEN; loopCount++)
			{
				if(membuf[PRODUCT_DISCOUNT + loopCount] != '.')
				{
					asc[storeCount++] = membuf[PRODUCT_DISCOUNT + loopCount];
				}
			}
			temprec.temprec.discount_amt = asc_hex(asc, PRODUCT_DISCOUNT_LEN - 1);

			//get product SGST
			for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_SGST_LEN; loopCount++)
			{
				if(membuf[PRODUCT_SGST + loopCount] != '.')
				{
					asc[storeCount++] = membuf[PRODUCT_SGST + loopCount];
				}
			}
			temprec.temprec.prd_sgst = asc_hex(asc, PRODUCT_SGST_LEN - 1);

			temprec.temprec.product_code = prodPosition;

			temprec.temprec.unit_type = searchUnit(asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN));

			double_width = 0;
			double_height = 0;
			gotoxy(8,1);
			if(!shiftFlag)
				lcdprint("             ENT/CNL");
			else
				lcdprint("SHIFT        ENT/CNL");
		}
		else
		{
			clrscr();
			double_height = 0;
			double_width = 1;
			gotoxy(1,1);
			lcdprint("   ITEMS  ");
			found = 0;
			double_height = 1;
			double_width = 0;
			gotoxy(2,1);
			hex_asc(entryIndex, asc, 4);
			for(loopCount = 0; loopCount < 4; loopCount++)
				disp_char(asc[loopCount]);

			gotoxy(5,1);
			lcdprint("   ITEM NOT FOUND   ");
			double_width = 0;
			double_height = 0;
			gotoxy(8,1);
			if(!shiftFlag)
				lcdprint("             ENT/CNL");
			else
				lcdprint("SHIFT        ENT/CNL");
		}

		scrollFlag = 0;

		while(!(key = keyPressCheck()));

		switch(key)
		{
		case DOWN_ARROW:
					if(scrollStart < mc.mc.prod_totalcount)
					{
						tempPos = scrollStart++;
						found = 0;
						while((!found) && (scrollStart <= mc.mc.prod_totalcount))
						{
							FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
							if(membuf[PRODUCT_EN_FLAG] != 1)
								found = 1;
							else
								scrollStart++;

						}
						if(!found)
						{
							scrollStart--;
							while((!found) && (scrollStart > 1))
							{
								FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
								if(membuf[PRODUCT_EN_FLAG] != 1)
									found = 1;
								else
									scrollStart--;
							}
						}
						if(!found)
							scrollStart = tempPos;
						scrollFlag = 1;
					}
					else
					{
						tempPos = scrollStart;
						scrollFlag = 1;
						scrollStart = 1;
						found = 0;
						while((!found) && (scrollStart <= mc.mc.prod_totalcount))
						{
							FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
							if(membuf[PRODUCT_EN_FLAG] != 1)
								found = 1;
							else
								scrollStart++;

						}
						if(!found)
							scrollStart = tempPos;
						scrollFlag = 1;
					}
					break;
		case BACK_SPACE:

						if(shiftFlag)
						{
							if(countSI > 0)
								countSI--;
							if(countSI >= 0)
								tempArr[countSI] = ' ';
							if(countSI <= 0)
							{
								countSI = 0;
								tempArr[countSI] = ' ';

						}
						entryIndex = asc_hex(tempArr, countSI);
						scrollStart = 1;
					}
					else
					{
						if(scrollStart > 1)
						{
							tempPos = scrollStart--;
							found = 0;

							while((!found) && (scrollStart > 0))
							{
								FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
								if(membuf[PRODUCT_EN_FLAG] != 1)
									found = 1;
								else
									scrollStart--;
							}

							if(!found)
							{
								scrollStart++;
								while((!found) && (scrollStart <= mc.mc.prod_totalcount))
								{
									FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
									if(membuf[PRODUCT_EN_FLAG] != 1)
										found = 1;
									else
										scrollStart++;

								}
							}
							if(!found)
								scrollStart = tempPos;
							scrollFlag = 1;
						}
						else
						{
							scrollStart = mc.mc.prod_totalcount;
							scrollFlag = 1;
							found = 0;

							while((!found) && (scrollStart > 0))
							{
								FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)scrollStart * PRODUCT_LEN), membuf, PRODUCT_LEN);
								if(membuf[PRODUCT_EN_FLAG] != 1)
									found = 1;
								else
									scrollStart--;
							}
							if(!found)
								scrollStart = tempPos;
							scrollFlag = 1;
						}
					}
					break;
			case SHIFT_KEY:
						shiftFlag = ~shiftFlag;
						break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
						if(countSI < 4)
							tempArr[countSI++] = key;
						entryIndex = asc_hex(tempArr, countSI);
						scrollStart = 1;
						break;
			case '*':
						return constPrd;
			case '#':
						if((scrollStart <= mc.mc.prod_totalcount) && (scrollStart >= 1) && (found))
							return prodPosition;
						break;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char pwd_entry(unsigned long pwd)
{
	clrscr();
	double_height = 1;
	gotoxy(3, 1);
	lcdprint("   ENTER PASSWORD");
	double_height = 0;
	if((get_dig(6, 7, AGENT_PASSWORD_LEN, 0, 0,1) ==  pwd))
	{
		double_height = 1;
		return (1);
	}
	clrscr();
	double_height = 1;
	gotoxy(3, 1);
	lcdprint("  PASSWORD ERROR !");
	beep(ERRRBEEP);
	return (0);
}
//-------------------------------------------------------------------------
void editOrderFromUnsavedKot(void)
{
	UCHAR exitMain = 0, exit = 0, count = 0, unit_func = 0, i = 0, key = 0, entry = 0, found = 0, sr_count = 1, total_sr = 0;
	UINT32 loopStart = 0, dispRec = 0, startRec = 0;
	UINT16 j = 0, tempRecArrayPrt = 0;

	total_sr = recArrayPtr;
	exit = 0;

	while(!exitMain)
	{
		found = 0;
		for(loopStart = 0; loopStart < recArrayPtr; )
		{
			for(i = 0; i < RECDAT_LEN; i++)
			{
				temprec.arr[i] = recArray[loopStart][i];
			}
			entry = 0;
			if(mc.mc.udf_en_dis)
			{
				if((temprec.temprec.bill_stat == 'O') &&(temprec.temprec.udf_no == udf_no))
				{
					entry = 1;
					found++;
				}
			}
			else
			{
				if(temprec.temprec.bill_stat == 'O')
				{
					entry = 1;
					found++;
				}
			}

			if(entry)
			{
				dispRec = loopStart;

				clrscr();
				double_height = 1;
				double_width = 0;
				gotoxy(1,1);
				hex_asc(sr_count, asc, 3);
				rem_zeros(asc, 2);
				for(count = 0; count < 3; count++)
					disp_char(asc[count]);
				disp_char(',');
				FlashReadData(((unsigned long)PRODUCT_PG * PGBYTS) + ((unsigned long) temprec.temprec.product_code * PRODUCT_LEN) + PRODUCT_TEXT, membuf, PRODUCT_TEXT_LEN);
				for(count = 0; count < (16); count++)
					disp_char(membuf[count]);

				FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_FRAC_QTY), membuf, UNIT_TYPE_FRAC_QTY_LEN);
				unit_func = asc_hex(membuf, UNIT_TYPE_FRAC_QTY_LEN);
				gotoxy(3,1);
				disp_char('Q');
				disp_char('T');
				disp_char('Y');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				displayQuantity(temprec.temprec.quantity, unit_func);
				FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), asc, UNIT_TYPE_SYMBOL_LEN);
				for(i = 0; i < UNIT_TYPE_SYMBOL_LEN - 3;i++)
					disp_char(asc[i]);
				gotoxy(5,1);
				disp_char('R');
				disp_char('A');
				disp_char('T');
				disp_char('E');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				hex_asc(temprec.temprec.rate, asc, 8);
				rem_zeros(asc,4);
				for(i = 0; i < 8; i++)
				{
					if(i == 6)
						disp_char('.');
					disp_char(asc[i]);
				}
				double_height = 0;
				double_width = 0;
				gotoxy(7,1);
				disp_char('A');
				disp_char('M');
				disp_char('T');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				value_hex_asc_64U = temprec.temprec.final_prod_amount;
				hex_asc_64U(asc, 11);
				rem_zeros(asc,7);
				for(i = 0; i < 11; i++)
				{
					if(i == 9)
						disp_char('.');
					disp_char(asc[i]);
				}

				gotoxy(8,1);
				lcdprint("PRESS   ENTER/CANCEL");
				while(!(key = keyPressCheck()));

				switch(key)
				{
				case UP_ARROW:
									if(dispRec > 0)
									{
										exit = 0;
										loopStart--;
										sr_count--;

										if(loopStart < startRec)
											loopStart = 0;
									}
									else
									{
										loopStart = recArrayPtr - 1;
										sr_count = total_sr;
									}
									break;
					case DOWN_ARROW:
									if(dispRec < (recArrayPtr - 1))
									{
										exit = 0;
										loopStart++;
										sr_count++;
									}
									else
									{
										loopStart = 0;
										sr_count = 1;
									}
									break;
					case '*':
									exitMain = 1;
									return;
					case '#':
									tempRecArrayPrt = recArrayPtr;
									customizeSelectedProductUnSavedKot(dispRec);
									selectedAmount = 0;
									selectedQty = 0;
									selectedProd = 0;
									total_sr = 0;
									for(j = 0; j < recArrayPtr; j++)
									{
										for(i = 0; i < RECDAT_LEN; i++)
										{
											temprec.arr[i] = recArray[j][i];
										}
										if(temprec.temprec.bill_stat == 'O')
										{
											selectedAmount += temprec.temprec.final_prod_amount;

											if(!mc.mc.reverse_tax_en_dis)		//Mrugnil 15/04/25
											{
												selectedAmount += temprec.temprec.sgst_amt;		//Mrugnil 15/04/25
												selectedAmount += temprec.temprec.cgst_amt;		//Mrugnil 15/04/25
											}
											if(temprec.temprec.discount_amt)		//Mrugnil 15/04/25
												selectedAmount -= temprec.temprec.discount_amt;		//Mrugnil 15/04/25

											selectedQty += temprec.temprec.quantity;
											selectedProd++;
											total_sr++;
										}
									}

									if(tempRecArrayPrt != recArrayPtr)
									{
										sr_count = loopStart + 1;
									}

									if(!selectedQty)
										recArrayPtr = 0;
									exit = 0;
									if(!exit)
										exitMain = 1;
									break;
					default:
									break;
				}
			}
			else
				loopStart++;
		}
		if(!found)
			exitMain = 1;
		CheckOnOffKey();
	}
}
//-------------------------------------------------------------------------
void editOrder(void)
{
	UCHAR exitMain = 0, exit = 0, count = 0, unit_func = 0, i = 0, key = 0, entry = 0, found = 0;
	UINT32 loopRecs = 0, loopStart = 0, dispRec = 0, startRec = 0, endRec = 0;

	if(mc.mc.udf_en_dis)
	{
		get_udf(udf_no);

		if(udfno.udfno.udfActive == 1)
		{
			loopStart = udfno.udfno.udfStart;
			startRec = loopStart;
		}
		else
			return;
	}
	else
	{
		if(mc.mc.rec_start)
			loopStart = mc.mc.rec_start;
		else
			return;
	}

	if(mc.mc.currentPG)
		endRec = mc.mc.recs;
	else
		endRec = mc.mc.rec1;

	if((!mc.mc.udf_en_dis)&&(!mc.mc.kot_en_dis))
	{
		for(loopRecs = (mc.mc.rec_start); (loopRecs < endRec) && (!exit); loopRecs++)
		{
			get_rec(loopRecs);

			if((rec.rec.bill_stat == 'O'))
			{
				exit = 1;
				loopStart = loopRecs;
				startRec = loopStart;
			}
		}
	}

	if(startRec == endRec)
		return;

	exit = 0;

	while(!exitMain)
	{
		found = 0;
		for(loopStart = startRec; loopStart < endRec; )
		{
			entry = 0;
			get_rec(loopStart);
			if(mc.mc.udf_en_dis)
			{
				if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no))
				{
					entry = 1;
					found++;
				}
			}
			else
			{
				if(rec.rec.bill_stat == 'O')
				{
					entry = 1;
					found++;
				}
			}

			if(entry)
			{
				dispRec = loopStart;

				clrscr();
				double_height = 1;
				double_width = 0;

				gotoxy(1,1);
				FlashReadData(((unsigned long)PRODUCT_PG * PGBYTS) + ((unsigned long) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_TEXT, membuf, PRODUCT_TEXT_LEN);
				for(count = 0; count < (PRODUCT_TEXT_LEN - 8); count++)
					disp_char(membuf[count]);

				FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)rec.rec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_FRAC_QTY), membuf, UNIT_TYPE_FRAC_QTY_LEN);
				unit_func = asc_hex(membuf, UNIT_TYPE_FRAC_QTY_LEN);
				gotoxy(3,1);
				disp_char('Q');
				disp_char('T');
				disp_char('Y');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				displayQuantity(rec.rec.quantity, unit_func);
				FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)rec.rec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), asc, UNIT_TYPE_SYMBOL_LEN);
				for(i = 0; i < UNIT_TYPE_SYMBOL_LEN - 3;i++)
					disp_char(asc[i]);
				gotoxy(5,1);
				disp_char('R');
				disp_char('A');
				disp_char('T');
				disp_char('E');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				hex_asc(rec.rec.rate, asc, 8);
				rem_zeros(asc,4);
				for(i = 0; i < 8; i++)
				{
					if(i == 6)
						disp_char('.');
					disp_char(asc[i]);
				}
				double_height = 0;
				double_width = 0;
				gotoxy(7,1);
				disp_char('A');
				disp_char('M');
				disp_char('T');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				value_hex_asc_64U = rec.rec.final_prod_amount;
				hex_asc_64U(asc, 11);
				rem_zeros(asc,7);
				for(i = 0; i < 11; i++)
				{
					if(i == 9)
						disp_char('.');
					disp_char(asc[i]);
				}

				gotoxy(8,1);
				lcdprint("PRESS   ENTER/CANCEL");
				while(!(key = keyPressCheck()));

				switch(key)
				{
				case UP_ARROW:
									if(dispRec > 0)
									{
										exit = 0;

										loopStart--;

										if(loopStart < startRec)
											loopStart = startRec;
									}
									break;
					case DOWN_ARROW:
									if(dispRec < (endRec - 1))
									{
										exit = 0;
										loopStart++;
									}
									break;
					case '*':
									exitMain = 1;
									return;
					case '#':
									customizeSelectedProduct(dispRec);
									exit = 0;
									if(mc.mc.udf_en_dis)
									{
										for(loopRecs = dispRec; (loopRecs < endRec) && (!exit); loopRecs++)
										{
											get_rec(loopRecs);

											if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no))
											{
												exit = 1;
												loopRecs--;
											}
										}
										if(!exit)
										{
											for(loopRecs = dispRec+1; (loopRecs < endRec) && (!exit); loopRecs++)
											{
												get_rec(loopRecs);

												if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no))
												{
													exit = 1;
													loopStart = loopRecs;
												}
											}
										}
										selectedAmount = 0;
										selectedQty = 0;
										for(loopRecs = startRec; (loopRecs < endRec); loopRecs++)
										{
											get_rec(loopRecs);

											if((rec.rec.bill_stat == 'O') && (rec.rec.udf_no == udf_no))
											{
												selectedAmount += rec.rec.final_prod_amount;

												if(!mc.mc.reverse_tax_en_dis)		//Mrugnil 15/04/25
												{
													selectedAmount += rec.rec.sgst_amt;		//Mrugnil 15/04/25
													selectedAmount += rec.rec.cgst_amt;		//Mrugnil 15/04/25
												}
												if(rec.rec.discount_amt)		//Mrugnil 15/04/25
													selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25

												selectedQty += rec.rec.quantity;
											}
										}
										edit_kot_qty_selected = 0;
										edit_kot_record_start = 0;
										edit_kot_record_end = 0; //vikas
										edit_kot_qty_flag = 0;
										if(selectedQty)//vikas
										{
											edit_kot_record_start = startRec;//vikas
											for(loopRecs = startRec; (loopRecs < endRec); loopRecs++)
											{
												get_rec(loopRecs);

												if((rec.rec.bill_stat == 'O') && (rec.rec.udf_char == udf_char) && (rec.rec.udf_no == udf_no))
												{
//													selectedAmount += rec.rec.final_prod_amount;
													edit_kot_qty_selected += rec.rec.quantity;
												}
											}
											edit_kot_record_end = edit_kot_qty_selected;
										}
									}
									else
									{
										for(loopRecs = dispRec; (loopRecs < endRec) && (!exit); loopRecs++)
										{
											get_rec(loopRecs);

											if((rec.rec.bill_stat == 'O'))
											{
												exit = 1;
												loopStart = loopRecs;
											}
										}
										if(!exit)
										{
											for(loopRecs = dispRec+1; (loopRecs < endRec) && (!exit); loopRecs++)
											{
												get_rec(loopRecs);

												if((rec.rec.bill_stat == 'O'))
												{
													exit = 1;
													loopStart = loopRecs;
												}
											}
										}
										selectedAmount = 0;
										selectedQty = 0;
										for(loopRecs = startRec; (loopRecs < endRec); loopRecs++)
										{
											get_rec(loopRecs);

											if((rec.rec.bill_stat == 'O'))
											{
												selectedAmount += rec.rec.final_prod_amount;

												if(!mc.mc.reverse_tax_en_dis)		//Mrugnil 15/04/25
												{
													selectedAmount += rec.rec.sgst_amt;		//Mrugnil 15/04/25
													selectedAmount += rec.rec.cgst_amt;		//Mrugnil 15/04/25
												}
												if(rec.rec.discount_amt)		//Mrugnil 15/04/25
													selectedAmount -= rec.rec.discount_amt;		//Mrugnil 15/04/25

												selectedQty += rec.rec.quantity;
											}
										}
									}
									if(!selectedQty)
									{
										recArrayPtr = 0;
										if(mc.mc.udf_en_dis)
										{
											get_udf(udf_no);
											udfno.udfno.udfActive = 0;
											udfno.udfno.udfRcptNo = 0;
											udfno.udfno.udfStart = 0;
											store_udf(udf_no);

											get_udf_prd_cnt(udf_no);
											udf_prd_cnt.udf_prd_cnt.prd_count = 0;
											store_udf_prd_cnt(udf_no);
											udf_no = 0;
										}
										else
										{
											mc.mc.rec_start = 0;
											mc.mc.selected = 0;
										}
									}
									if(!exit)
										exitMain = 1;
									break;
					default:
									break;
				}
			}
			else
				loopStart++;
		}
		if(!found)
			exitMain = 1;
		CheckOnOffKey();
	}
}
//-------------------------------------------------------------------------
void customizeSelectedProduct(unsigned long recPosition)
{
	unsigned char exit1 = 0, stepCount = 0, key = 0, count = 0, unit_func = 0, i = 0;
	unsigned long long tempvar64U = 0;

	get_rec(recPosition);

	for(i = 0; i < RECDAT_LEN; i++)
		temprec.arr[i] = rec.arr[i];

	while(!exit1)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("SELECT PRODUCT ");
		double_height = 0;
		double_width = 0;

		gotoxy(3,2);
		FlashReadData(((unsigned long)PRODUCT_PG * PGBYTS) + ((unsigned long) temprec.temprec.product_code * PRODUCT_LEN) + PRODUCT_TEXT, membuf, PRODUCT_LEN);
		for(count = 0; count < (PRODUCT_TEXT_LEN - 8); count++)
			disp_char(membuf[count]);

		FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_FRAC_QTY), membuf, UNIT_TYPE_FRAC_QTY_LEN);
		unit_func = asc_hex(membuf, UNIT_TYPE_FRAC_QTY_LEN);
		gotoxy(4,2);
		lcdprint("QTY : ");
		displayQuantity(temprec.temprec.quantity, unit_func);
		FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), asc, UNIT_TYPE_SYMBOL_LEN);
		for(i = 0; i < UNIT_TYPE_SYMBOL_LEN - 3;i++)
			disp_char(asc[i]);

		gotoxy(5,2);
		lcdprint("RATE :  ");
		hex_asc(temprec.temprec.rate, asc, 8);
		rem_zeros(asc,4);
		for(i = 0; i < 8; i++)
		{
			if(i == 6)
				disp_char('.');
			disp_char(asc[i]);
		}
		gotoxy(6,1);
		lcdprint(" TOT :");
		value_hex_asc_64U = temprec.temprec.final_prod_amount;
		hex_asc_64U(asc, 11);
		rem_zeros(asc,7);
		for(i = 0; i < 11; i++)
		{
			if(i == 9)
				disp_char('.');
			disp_char(asc[i]);
		}

		gotoxy(7,2);
		lcdprint("SAVE             ");
		gotoxy(8,2);
		lcdprint("DELETE FROM ORDER");


		switch(stepCount)
		{
			case 0:
					gotoxy(3,1);
					disp_char('>');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					gotoxy(8,1);
					disp_char(' ');
					break;
			case 1:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char('>');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					gotoxy(8,1);
					disp_char(' ');
					break;
			case 2:
					if(mc.mc.modify_rate_enable)
					{
						gotoxy(3,1);
						disp_char(' ');
						gotoxy(4,1);
						disp_char(' ');
						gotoxy(5,1);
						disp_char('>');
						gotoxy(7,1);
						disp_char(' ');
						gotoxy(8,1);
						disp_char(' ');
					}
					break;
			case 3:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char('>');
					gotoxy(8,1);
					disp_char(' ');
					break;
			case 4:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					gotoxy(8,1);
					disp_char('>');
					break;
		}

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case UP_ARROW:
							if(stepCount > 0)
									stepCount--;
							if(!mc.mc.modify_rate_enable)
							{
								if(stepCount == 2)
									stepCount--;
							}
							break;
			case DOWN_ARROW:
							if(stepCount < 4)
								stepCount++;
							if(!mc.mc.modify_rate_enable)
							{
								if(stepCount == 2)
									stepCount++;
							}
							break;
			case '*':
							exit1 = 1;
							break;
			case '#':
							switch(stepCount)
							{
								case 0:
										temprec.temprec.product_code = productEnteryEdit(temprec.temprec.product_code);

										tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
										temprec.temprec.final_prod_amount = (tempvar64U);
										break;
								case 1:
										temprec.temprec.quantity = get_QUANTITY(1);
										tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
										temprec.temprec.final_prod_amount = (tempvar64U);
										break;
								case 2:
										if(mc.mc.modify_rate_enable)
										{
											temprec.temprec.rate = enterManuaRate(temprec.temprec.rate);
											temprec.temprec.manualRate = temprec.temprec.rate;
											tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
											temprec.temprec.final_prod_amount = (tempvar64U);
										}
										break;
								case 3:
										recalculateAndStoreEditedRecords(recPosition);
										exit1 = 1;
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("UPDATED SUCCESSFULLY");
										beep(OKKKBEEP);
										Delay_ms(2000);
										break;
								case 4:
										rec.rec.bill_stat = 'D';
										rec.rec.strt_byt = 'D';
										store_rec(recPosition);
										get_udf_prd_cnt(udf_no);
										udf_prd_cnt.udf_prd_cnt.prd_count--;
										store_udf_prd_cnt(udf_no);
										exit1 = 1;
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("REMOVED SUCCESSFULLY");
										beep(OKKKBEEP);
										Delay_ms(2000);
										break;
							}
							break;
			default:
							break;
		}
	}

}
//-------------------------------------------------------------------------
void customizeSelectedProductUnSavedKot(unsigned char recPosition)
{
	unsigned char exit1 = 0, stepCount = 0, key = 0, count = 0, unit_func = 0, i = 0, j = 0;
	unsigned long long tempvar64U = 0;

	for(i = 0; i < RECDAT_LEN; i++)
		temprec.arr[i] = recArray[recPosition][i];

	while(!exit1)
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("SELECT PRODUCT ");
		double_height = 0;
		double_width = 0;

		gotoxy(3,2);
		FlashReadData(((unsigned long)PRODUCT_PG * PGBYTS) + ((unsigned long) temprec.temprec.product_code * PRODUCT_LEN) + PRODUCT_TEXT, membuf, PRODUCT_LEN);
		for(count = 0; count < (PRODUCT_TEXT_LEN - 8); count++)
			disp_char(membuf[count]);

		FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_FRAC_QTY), membuf, UNIT_TYPE_FRAC_QTY_LEN);
		unit_func = asc_hex(membuf, UNIT_TYPE_FRAC_QTY_LEN);
		gotoxy(4,2);
		lcdprint("QTY : ");
		displayQuantity(temprec.temprec.quantity, unit_func);
		FlashReadData((((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)temprec.temprec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), asc, UNIT_TYPE_SYMBOL_LEN);
		for(i = 0; i < UNIT_TYPE_SYMBOL_LEN - 3;i++)
			disp_char(asc[i]);

		gotoxy(5,2);
		lcdprint("RATE :  ");
		hex_asc(temprec.temprec.rate, asc, 8);
		rem_zeros(asc,4);
		for(i = 0; i < 8; i++)
		{
			if(i == 6)
				disp_char('.');
			disp_char(asc[i]);
		}
		gotoxy(6,1);
		lcdprint(" TOT :");
		value_hex_asc_64U = temprec.temprec.final_prod_amount;
		hex_asc_64U(asc, 11);
		rem_zeros(asc,7);
		for(i = 0; i < 11; i++)
		{
			if(i == 9)
				disp_char('.');
			disp_char(asc[i]);
		}

		gotoxy(7,2);
		lcdprint("SAVE             ");
		gotoxy(8,2);
		lcdprint("DELETE FROM ORDER");


		switch(stepCount)
		{
			case 0:
					gotoxy(3,1);
					disp_char('>');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					gotoxy(8,1);
					disp_char(' ');
					break;
			case 1:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char('>');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					gotoxy(8,1);
					disp_char(' ');
					break;
			case 2:
					if(mc.mc.modify_rate_enable)
					{
						gotoxy(3,1);
						disp_char(' ');
						gotoxy(4,1);
						disp_char(' ');
						gotoxy(5,1);
						disp_char('>');
						gotoxy(7,1);
						disp_char(' ');
						gotoxy(8,1);
						disp_char(' ');
					}
					break;
			case 3:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char('>');
					gotoxy(8,1);
					disp_char(' ');
					break;
			case 4:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					gotoxy(8,1);
					disp_char('>');
					break;
		}

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case UP_ARROW:
							if(stepCount > 0)
									stepCount--;
							if(!mc.mc.modify_rate_enable)
							{
								if(stepCount == 2)
									stepCount--;
							}
							break;
			case DOWN_ARROW:
							if(stepCount < 4)
								stepCount++;
							if(!mc.mc.modify_rate_enable)
							{
								if(stepCount == 2)
									stepCount++;
							}
							break;
			case '*':
							exit1 = 1;
							break;
			case '#':
							switch(stepCount)
							{
								case 0:
										temprec.temprec.product_code = productEnteryEdit(temprec.temprec.product_code);

										tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
										temprec.temprec.final_prod_amount = (tempvar64U);
										break;
								case 1:
										temprec.temprec.quantity = get_QUANTITY(1);
										tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
										temprec.temprec.final_prod_amount = (tempvar64U);
										break;
								case 2:
										if(mc.mc.modify_rate_enable)
										{
											temprec.temprec.rate = enterManuaRate(temprec.temprec.rate);
											temprec.temprec.manualRate = temprec.temprec.rate;
											tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
											temprec.temprec.final_prod_amount = (tempvar64U);
										}
										break;
								case 3:
										recalculateEditedRecords();
										for(i = 0; i < RECDAT_LEN; i++)
											recArray[recPosition][i] = temprec.arr[i];
										exit1 = 1;
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("UPDATED SUCCESSFULLY");
										beep(OKKKBEEP);
										Delay_ms(2000);
										break;
								case 4:
										temprec.temprec.bill_stat = 'D';
										temprec.temprec.strt_byt = 'D';
										for(i = 0; i < RECDAT_LEN; i++)
											recArray[recPosition][i] = temprec.arr[i];

										for(j = recPosition; j < (recArrayPtr - 1); j++)
										{
											for(i = 0; i < RECDAT_LEN; i++)
												temprec.arr[i] = recArray[j + 1][i];

											if(temprec.temprec.bill_stat == 'O')
											{
												for(i = 0; i < RECDAT_LEN; i++)
													recArray[j][i] = temprec.arr[i];
											}
										}

										recArrayPtr--;

										exit1 = 1;
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("REMOVED SUCCESSFULLY");
										beep(OKKKBEEP);
										Delay_ms(2000);
										break;
							}
							break;
			default:
							break;
		}
	}

}
//-------------------------------------------------------------------------
void recalculateAndStoreEditedRecords(unsigned long recpos)
{
	UINT32 tem_disc = 0;
	UINT64 temp_amount = 0, final_discount_amt = 0, sgstTemp = 0, cgstTemp = 0, tempRate = 0;
	UCHAR temp_arr[PRODUCT_LEN] = {0}, i = 0, j = 0, startDef = 0, exit = 0;

	read_rtc();

	if(mc.mc.discount_en == ENABLE)
	{
		if(mc.mc.discount_type == ITEMWISE)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			for(i = 0, j = PRODUCT_DISCOUNT; j < (PRODUCT_DISCOUNT + PRODUCT_DISCOUNT_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			if(mc.mc.iw_disc_percent_auto_manual == 1)
			{
//				if(temp_amount)
//				{
//					final_discount_amt = 0;
//					final_discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount, temp_amount, DISCOUNT);
//
//					if(final_discount_amt)
//						temprec.temprec.discount_amt = final_discount_amt;
//				}
//				else
//				{
					clrscr();
					double_height = 1;
					gotoxy(1,1);
					lcdprint("ENTER DISCOUNT IN %");
					get_dig_dot_esc_key = 0;
					recordedDotPosition = 0;
					temp_amount = get_dig_dot(4,9,2,2,0,1);
					if(!get_dig_dot_esc_key)
					{
						if(temp_amount)
						{
							final_discount_amt = 0;
							final_discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount, temp_amount, DISCOUNT);

							if(final_discount_amt)
							{
								temprec.temprec.disc_rate = temp_amount;//vikas
								temprec.temprec.discount_amt = final_discount_amt;
							}
						}
						else
							temprec.temprec.discount_amt = 0;
					}
					else
						temprec.temprec.discount_amt = 0;
//				}
			}
			else
			{
				final_discount_amt = 0;
				temprec.temprec.disc_rate = temp_amount;//vikas
				final_discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount, temp_amount, DISCOUNT);
				temprec.temprec.discount_amt = final_discount_amt;//vikas
			}

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;

			if(mc.mc.reverse_tax_en_dis)
			{
				if(!mc.mc.modify_rate_enable)
				{
					FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
					switch(temprec.temprec.slab_no)
					{
						case 1:
							startDef = PRODUCT_AMOUNT;
								break;
						case 2:
							startDef = PRODUCT_AMOUNT2;
								break;
						case 3:
							startDef = PRODUCT_AMOUNT3;
								break;
					}
					for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
					{
						if(membuf[j] != '.')
							temp_arr[i++] = membuf[j];
					}
					temprec.temprec.rate = asc_hex(temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				else
				{
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, MRP);
				}
				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, MRP);
				}
			}
			else
			{
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, TAX);
				}
				else
					temprec.temprec.sgst_amt = 0;

				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, TAX);
				}
				else
					temprec.temprec.cgst_amt = 0;
			}
		}
		else if(mc.mc.discount_type == ITEMWISE_RUPEE)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			exit = 0;
			while(!exit)
			{
				clrscr();
				double_height = 1;
				gotoxy(1,1);
				lcdprint("ENTER DISCOUNT AMT.");
				hex_asc(temprec.temprec.final_prod_amount, asc, 9);
				rem_zeros(asc, 5);
				gotoxy(3,1);
				lcdprint("AMT: ");
				for(i = 0; i < 9; i++)
				{
					if(i == 7)
						disp_char('.');
					disp_char(asc[i]);
				}
				get_dig_dot_esc_key = 0;
				recordedDotPosition = 0;
				tem_disc = get_dig_dot(6,9,5,2,0,1);
				if(!get_dig_dot_esc_key)
				{
					if(tem_disc)
					{
						if(tem_disc <= temprec.temprec.final_prod_amount)
						{
							temprec.temprec.disc_rate = floatCalculatePercentageU64(temprec.temprec.final_prod_amount, tem_disc);//vikas
							temprec.temprec.discount_amt = tem_disc;
							exit = 1;
						}
						else
						{
							clrscr();
							double_height = 1;
							gotoxy(2,1);
							lcdprint(" DISCOUNT EXCEEDING ");
							gotoxy(4,1);
							lcdprint("   PRODUCT AMOUNT   ");
							beep(ERRRBEEP);
							Delay_ms(1000);
						}
					}
					else
					{
						temprec.temprec.discount_amt = 0;
						exit = 1;
					}
				}
				else
				{
					temprec.temprec.discount_amt = 0;
					exit = 1;
				}
			}

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;

			if(mc.mc.reverse_tax_en_dis)
			{
				if(!mc.mc.modify_rate_enable)
				{
					FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
					switch(temprec.temprec.slab_no)
					{
						case 1:
							startDef = PRODUCT_AMOUNT;
								break;
						case 2:
							startDef = PRODUCT_AMOUNT2;
								break;
						case 3:
							startDef = PRODUCT_AMOUNT3;
								break;
					}
					for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
					{
						if(membuf[j] != '.')
							temp_arr[i++] = membuf[j];
					}
					temprec.temprec.rate = asc_hex(temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				else
				{
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}

				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, MRP);
				}
				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, MRP);
				}
			}
			else
			{
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, TAX);
				}
				else
					temprec.temprec.sgst_amt = 0;

				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, TAX);
				}
				else
					temprec.temprec.cgst_amt = 0;
			}
		}
		else if(mc.mc.discount_type == PRE_TAX)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;
			temprec.temprec.sgst_amt = 0;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;
			temprec.temprec.cgst_amt = 0;
		}
		else
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;

			if(mc.mc.reverse_tax_en_dis)
			{
				if(!mc.mc.modify_rate_enable)
				{
					FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
					switch(temprec.temprec.slab_no)
					{
						case 1:
							startDef = PRODUCT_AMOUNT;
								break;
						case 2:
							startDef = PRODUCT_AMOUNT2;
								break;
						case 3:
							startDef = PRODUCT_AMOUNT3;
								break;
					}
					for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
					{
						if(membuf[j] != '.')
							temp_arr[i++] = membuf[j];
					}
					temprec.temprec.rate = asc_hex(temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				else
				{
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}

				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, MRP);
				}
				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, MRP);
				}
			}
			else
			{
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, TAX);
				}
				else
					temprec.temprec.sgst_amt = 0;

				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, TAX);
				}
				else
					temprec.temprec.cgst_amt = 0;
			}
		}
	}
	else
	{
		temprec.temprec.discount_amt = 0;

		FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
		for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
		{
			if(membuf[j] != '.')
				temp_arr[i++] = membuf[j];
		}
		temp_amount = 0;
		temp_amount = asc_hex(temp_arr, i);
		temprec.temprec.prd_sgst = temp_amount;

		FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
		for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
		{
			if(membuf[j] != '.')
				temp_arr[i++] = membuf[j];
		}
		temp_amount = 0;
		temp_amount = asc_hex(temp_arr, i);
		temprec.temprec.prd_cgst = temp_amount;

		if(mc.mc.reverse_tax_en_dis)
		{
			if(!mc.mc.modify_rate_enable)
			{
				FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
				switch(temprec.temprec.slab_no)
				{
					case 1:
						startDef = PRODUCT_AMOUNT;
							break;
					case 2:
						startDef = PRODUCT_AMOUNT2;
							break;
					case 3:
						startDef = PRODUCT_AMOUNT3;
							break;
				}
				for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
				{
					if(membuf[j] != '.')
						temp_arr[i++] = membuf[j];
				}
				temprec.temprec.rate = asc_hex(temp_arr, (PRODUCT_AMOUNT_LEN - 1));
				final_bill_amount = temprec.temprec.rate;
				qty = temprec.temprec.quantity;
				temprec.temprec.final_prod_amount = calculate_rate();
			}
			else
			{
				final_bill_amount = temprec.temprec.rate;
				qty = temprec.temprec.quantity;
				temprec.temprec.final_prod_amount = calculate_rate();
			}
			if(temprec.temprec.prd_sgst)
			{
				temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, MRP);
			}
			if(temprec.temprec.prd_cgst)
			{
				temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, MRP);
			}
		}
		else
		{
			if(temprec.temprec.prd_sgst)
			{
				temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, TAX);
			}
			else
				temprec.temprec.sgst_amt = 0;

			if(temprec.temprec.prd_cgst)
			{
				temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, TAX);
			}
			else
				temprec.temprec.cgst_amt = 0;
		}
	}

	for(i = 0; i < RECDAT_LEN; i++)
		rec.arr[i] = temprec.arr[i];

	store_rec(recpos);
}
//-------------------------------------------------------------------------
void recalculateEditedRecords(void)
{
	UINT32 tem_disc = 0;
	UINT64 temp_amount = 0, final_discount_amt = 0, sgstTemp = 0, cgstTemp = 0, tempRate = 0;
	UCHAR temp_arr[PRODUCT_LEN] = {0}, i = 0, j = 0, startDef = 0, exit = 0;

	read_rtc();

	if(mc.mc.discount_en == ENABLE)
	{
		if(mc.mc.discount_type == ITEMWISE)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			for(i = 0, j = PRODUCT_DISCOUNT; j < (PRODUCT_DISCOUNT + PRODUCT_DISCOUNT_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			if(mc.mc.iw_disc_percent_auto_manual == 1)
			{
//				if(temp_amount)
//				{
//					final_discount_amt = 0;
//					final_discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount, temp_amount, DISCOUNT);
//
//					if(final_discount_amt)
//						temprec.temprec.discount_amt = final_discount_amt;
//				}
//				else
//				{
					clrscr();
					double_height = 1;
					gotoxy(1,1);
					lcdprint("ENTER DISCOUNT IN %");
					get_dig_dot_esc_key = 0;
					recordedDotPosition = 0;
					temp_amount = get_dig_dot(4,9,2,2,0,1);
					if(!get_dig_dot_esc_key)
					{
						if(temp_amount)
						{
							final_discount_amt = 0;
							final_discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount, temp_amount, DISCOUNT);

							if(final_discount_amt)
							{
								temprec.temprec.disc_rate = temp_amount;//vikas
								temprec.temprec.discount_amt = final_discount_amt;
							}
						}
						else
							temprec.temprec.discount_amt = 0;
					}
					else
						temprec.temprec.discount_amt = 0;
//				}
			}
			else
			{
				final_discount_amt = 0;
				temprec.temprec.disc_rate = temp_amount;//vikas
				final_discount_amt = floatExtractionU64(temprec.temprec.final_prod_amount, temp_amount, DISCOUNT);
				temprec.temprec.discount_amt = final_discount_amt;//vikas
			}

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;

			if(mc.mc.reverse_tax_en_dis)
			{
				if(!mc.mc.modify_rate_enable)
				{
					FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
					switch(temprec.temprec.slab_no)
					{
						case 1:
							startDef = PRODUCT_AMOUNT;
								break;
						case 2:
							startDef = PRODUCT_AMOUNT2;
								break;
						case 3:
							startDef = PRODUCT_AMOUNT3;
								break;
					}
					for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
					{
						if(membuf[j] != '.')
							temp_arr[i++] = membuf[j];
					}
//					temprec.temprec.rate = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					final_bill_amount = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					if(!final_bill_amount)
					{
						final_bill_amount = temprec.temprec.rate;
					}
					else
					{
						temprec.temprec.rate = final_bill_amount;
					}

//					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				else
				{
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, MRP);
				}
				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, MRP);
				}
			}
			else
			{
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, TAX);
				}
				else
					temprec.temprec.sgst_amt = 0;

				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, TAX);
				}
				else
					temprec.temprec.cgst_amt = 0;
			}
		}
		else if(mc.mc.discount_type == ITEMWISE_RUPEE)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);

			exit = 0;
			while(!exit)
			{
				clrscr();
				double_height = 1;
				gotoxy(1,1);
				lcdprint("ENTER DISCOUNT AMT.");
				hex_asc(temprec.temprec.final_prod_amount, asc, 9);
				rem_zeros(asc, 5);
				gotoxy(3,1);
				lcdprint("AMT: ");
				for(i = 0; i < 9; i++)
				{
					if(i == 7)
						disp_char('.');
					disp_char(asc[i]);
				}
				get_dig_dot_esc_key = 0;
				recordedDotPosition = 0;
				tem_disc = get_dig_dot(6,9,5,2,0,1);
				if(!get_dig_dot_esc_key)
				{
					if(tem_disc)
					{
						if(tem_disc <= temprec.temprec.final_prod_amount)
						{
							temprec.temprec.disc_rate = floatCalculatePercentageU64(temprec.temprec.final_prod_amount, tem_disc);//vikas
							temprec.temprec.discount_amt = tem_disc;
							exit = 1;
						}
						else
						{
							clrscr();
							double_height = 1;
							gotoxy(2,1);
							lcdprint(" DISCOUNT EXCEEDING ");
							gotoxy(4,1);
							lcdprint("   PRODUCT AMOUNT   ");
							beep(ERRRBEEP);
							Delay_ms(1000);
						}
					}
					else
					{
						temprec.temprec.discount_amt = 0;
						exit = 1;
					}
				}
				else
				{
					temprec.temprec.discount_amt = 0;
					exit = 1;
				}
			}

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;

			if(mc.mc.reverse_tax_en_dis)
			{
				if(!mc.mc.modify_rate_enable)
				{
					FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
					switch(temprec.temprec.slab_no)
					{
						case 1:
							startDef = PRODUCT_AMOUNT;
								break;
						case 2:
							startDef = PRODUCT_AMOUNT2;
								break;
						case 3:
							startDef = PRODUCT_AMOUNT3;
								break;
					}
					for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
					{
						if(membuf[j] != '.')
							temp_arr[i++] = membuf[j];
					}
//					temprec.temprec.rate = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					final_bill_amount = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					if(!final_bill_amount)
					{
						final_bill_amount = temprec.temprec.rate;
					}
					else
					{
						temprec.temprec.rate = final_bill_amount;
					}
//					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				else
				{
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}

				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, MRP);
				}
				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, MRP);
				}
			}
			else
			{
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_sgst, TAX);
				}
				else
					temprec.temprec.sgst_amt = 0;

				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount - temprec.temprec.discount_amt), temprec.temprec.prd_cgst, TAX);
				}
				else
					temprec.temprec.cgst_amt = 0;
			}
		}
		else if(mc.mc.discount_type == PRE_TAX)
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;
			temprec.temprec.sgst_amt = 0;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;
			temprec.temprec.cgst_amt = 0;
		}
		else
		{
			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_sgst = temp_amount;

			FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
			for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
			{
				if(membuf[j] != '.')
					temp_arr[i++] = membuf[j];
			}
			temp_amount = 0;
			temp_amount = asc_hex(temp_arr, i);
			temprec.temprec.prd_cgst = temp_amount;

			if(mc.mc.reverse_tax_en_dis)
			{
				if(!mc.mc.modify_rate_enable)
				{
					FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
					switch(temprec.temprec.slab_no)
					{
						case 1:
							startDef = PRODUCT_AMOUNT;
								break;
						case 2:
							startDef = PRODUCT_AMOUNT2;
								break;
						case 3:
							startDef = PRODUCT_AMOUNT3;
								break;
					}
					for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
					{
						if(membuf[j] != '.')
							temp_arr[i++] = membuf[j];
					}
//					temprec.temprec.rate = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					final_bill_amount = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
					if(!final_bill_amount)
					{
						final_bill_amount = temprec.temprec.rate;
					}
					else
					{
						temprec.temprec.rate = final_bill_amount;
					}
//					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}
				else
				{
					final_bill_amount = temprec.temprec.rate;
					qty = temprec.temprec.quantity;
					temprec.temprec.final_prod_amount = calculate_rate();
				}

				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, MRP);
				}
				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, MRP);
				}
			}
			else
			{
				if(temprec.temprec.prd_sgst)
				{
					temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, TAX);
				}
				else
					temprec.temprec.sgst_amt = 0;

				if(temprec.temprec.prd_cgst)
				{
					temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, TAX);
				}
				else
					temprec.temprec.cgst_amt = 0;
			}
		}
	}
	else
	{
		temprec.temprec.discount_amt = 0;

		FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
		for(i = 0, j = PRODUCT_SGST; j < (PRODUCT_SGST + PRODUCT_SGST_LEN); j++)
		{
			if(membuf[j] != '.')
				temp_arr[i++] = membuf[j];
		}
		temp_amount = 0;
		temp_amount = asc_hex(temp_arr, i);
		temprec.temprec.prd_sgst = temp_amount;

		FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
		for(i = 0, j = PRODUCT_CGST; j < (PRODUCT_CGST + PRODUCT_CGST_LEN); j++)
		{
			if(membuf[j] != '.')
				temp_arr[i++] = membuf[j];
		}
		temp_amount = 0;
		temp_amount = asc_hex(temp_arr, i);
		temprec.temprec.prd_cgst = temp_amount;

		if(mc.mc.reverse_tax_en_dis)
		{
			if(!mc.mc.modify_rate_enable)
			{
				FlashReadData((((UINT32)PRODUCT_PG * PGBYTS) + PRODUCT_SER_NO + ((UINT32)temprec.temprec.product_code * PRODUCT_LEN)), membuf, PRODUCT_LEN);
				switch(temprec.temprec.slab_no)
				{
					case 1:
						startDef = PRODUCT_AMOUNT;
							break;
					case 2:
						startDef = PRODUCT_AMOUNT2;
							break;
					case 3:
						startDef = PRODUCT_AMOUNT3;
							break;
				}
				for(i = 0, j = startDef; j < (startDef + PRODUCT_AMOUNT_LEN); j++)
				{
					if(membuf[j] != '.')
						temp_arr[i++] = membuf[j];
				}
//				temprec.temprec.rate = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
				final_bill_amount = asc_hex (temp_arr, (PRODUCT_AMOUNT_LEN - 1));
				if(!final_bill_amount)
				{
					final_bill_amount = temprec.temprec.rate;
				}
				else
				{
					temprec.temprec.rate = final_bill_amount;
				}
//				final_bill_amount = temprec.temprec.rate;
				qty = temprec.temprec.quantity;
				temprec.temprec.final_prod_amount = calculate_rate();
			}
			else
			{
				final_bill_amount = temprec.temprec.rate;
				qty = temprec.temprec.quantity;
				temprec.temprec.final_prod_amount = calculate_rate();
			}
			if(temprec.temprec.prd_sgst)
			{
				temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, MRP);
			}
			if(temprec.temprec.prd_cgst)
			{
				temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, MRP);
			}
		}
		else
		{
			if(temprec.temprec.prd_sgst)
			{
				temprec.temprec.sgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_sgst, TAX);
			}
			else
				temprec.temprec.sgst_amt = 0;

			if(temprec.temprec.prd_cgst)
			{
				temprec.temprec.cgst_amt = floatExtractionU64((temprec.temprec.final_prod_amount), temprec.temprec.prd_cgst, TAX);
			}
			else
				temprec.temprec.cgst_amt = 0;
		}
	}
}
//-------------------------------------------------------------------------
//original
void saveOrder(void)
{
	UINT16 count = 0, i = 0, recCountLoop = 0, final_amt = 0, preTaxDiscRate = 0;
	UINT32 endAdd = 0, dataCount = 0;
	UINT64 temp_amount = 0, vat_temp = 0, serv = 0, tempRate = 0, final_prod_amt = 0;
	UCHAR exit = 0;

//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint("   SAVING RECORDS   ");

	if(mc.mc.udf_en_dis && (!calculatorMode))
	{
		get_udf(udf_no);
		if((!udfno.udfno.udfActive))
		{
			udfno.udfno.udfActive = 1;
			if(mc.mc.currentPG)
				udfno.udfno.udfStart = mc.mc.recs;
			else
				udfno.udfno.udfStart = mc.mc.rec1;

			udfno.udfno.udfRcptNo = mc.mc.rcpt_no;
			store_udf(udf_no);

			if(mc.mc.rcpt_no >= MAX_RCPTNO)
				mc.mc.rcpt_no = 1;
			else
				mc.mc.rcpt_no++;
		}
		get_udf_prd_cnt(udf_no);
		if(!udf_prd_cnt.udf_prd_cnt.prd_count)
			udf_prd_cnt.udf_prd_cnt.prd_count = selectedProd;
		else
			udf_prd_cnt.udf_prd_cnt.prd_count += selectedProd;
		store_udf_prd_cnt(udf_no);
		rec.rec.rcpt_no = udfno.udfno.udfRcptNo;
	}
	else if(!calculatorMode)
	{
		rec.rec.rcpt_no = mc.mc.rcpt_no;
	}

	if((recArrayPtr > 0) && (selectedProd > 0))
	{
		if(mc.mc.currentPG)
			startProd = mc.mc.recs;
		else
			startProd = mc.mc.rec1;

		mc.mc.rec_start = startProd;
		endAdd = startProd;

		selectedProd = 0;

		if((!mc.mc.udf_en_dis) && (!calculatorMode)
				&& (mc.mc.discount_en == ENABLE) && (mc.mc.discount_type == PRE_TAX))
		{
			clrscr();
			double_height = 1;
			gotoxy(1,1);
			lcdprint("ENTER DISCOUNT IN %");
			get_dig_dot_esc_key = 0;
			recordedDotPosition = 0;
			final_amt = get_dig_dot(4,9,2,2,0,1);
			if(!get_dig_dot_esc_key)
			{
				if(final_amt)
				{
					preTaxDiscRate = final_amt;
				}
				else
				{
					preTaxDiscRate = 0;
				}
			}
			else
			{
				preTaxDiscRate = 0;
			}
		}
		else if((!mc.mc.udf_en_dis) && (!calculatorMode)
				&& (mc.mc.discount_en == ENABLE) && (mc.mc.discount_type == PRE_TAX_RUPEE))
		{
			for(count = 0, final_prod_amt = 0; count < recArrayPtr; count++)
			{
				for(i = 0; i < RECDAT_LEN; i++)
					rec.arr[i] = recArray[count][i];
				if(rec.rec.bill_stat == 'O')
				{
					final_prod_amt += rec.rec.final_prod_amount;
				}
			}
			while(!exit)
			{
				pretax_discount_rupee = 0;
				tempRate = 0;
				clrscr();
				double_height = 1;
				gotoxy(1,1);
				lcdprint("ENTER DISCOUNT IN Rs");
				get_dig_dot_esc_key = 0;
				recordedDotPosition = 0;
				tempRate = get_dig_dot(4,9,5,2,0,1);
				if(!get_dig_dot_esc_key)
				{
					if(tempRate < final_prod_amt)
					{
						pretax_discount_rupee = tempRate;
						preTaxDiscRate = floatCalculatePercentageU64(final_prod_amt, tempRate);
						exit = 1;
					}
					else
					{
						preTaxDiscRate = 0;
						clrscr();
						double_height = 1;
						gotoxy(3,1);
						lcdprint(" DISCOUNT SHOULD BE ");
						gotoxy(5,1);
						lcdprint(" LESS THAN FINAL AMT");
						beep(ERRRBEEP);
						Delay_ms(1000);
					}
				}
				else
				{
					preTaxDiscRate = 0;
					exit = 1;
				}
			}
		}
//		clrscr();
//		double_height = 1;
//		double_width = 0;
//		gotoxy(3,1);
//		lcdprint("   SAVING RECORDS   ");

	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
		for(count = 0, dataCount = 0, recCountLoop = 0; count < recArrayPtr; count++)
		{
			for(i = 0; i < RECDAT_LEN; i++)
				rec.arr[i] = recArray[count][i];
			if(rec.rec.bill_stat == 'O')
			{
				if((!mc.mc.udf_en_dis) && (!calculatorMode)
						&& (mc.mc.discount_en == ENABLE) && ((mc.mc.discount_type == PRE_TAX) || (mc.mc.discount_type == PRE_TAX_RUPEE)))
				{
					if((rec.rec.bill_stat == 'O'))
					{
						rec.rec.disc_rate = preTaxDiscRate;//vikas
						rec.rec.discount_amt = floatExtractionU64(rec.rec.final_prod_amount, preTaxDiscRate, DISCOUNT);

						if(mc.mc.reverse_tax_en_dis)
						{
							if(rec.rec.prd_sgst)
							{
								rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, MRP);
							}
							if(rec.rec.prd_cgst)
							{
								rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_cgst, MRP);
							}
						}
						else
						{
							if(rec.rec.prd_sgst)
							{
								rec.rec.sgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
							}
							else
								rec.rec.sgst_amt = 0;
							if(rec.rec.prd_cgst)
							{
								rec.rec.cgst_amt = floatExtractionU64((rec.rec.final_prod_amount - rec.rec.discount_amt), rec.rec.prd_sgst, TAX);
							}
							else
								rec.rec.cgst_amt = 0;
						}
					}
				}
				if(mc.mc.udf_en_dis && !calculatorMode)
				{
					rec.rec.rcpt_no = udfno.udfno.udfRcptNo;
					if(((print_mode == 1) || (print_mode == 3)) && (mc.mc.billKotseries == DIFFERENT_SERIES) && (mc.mc.kot_en_dis))
						rec.rec.kot_no = mc.mc.kot_no;
					else
						rec.rec.kot_no = rec.rec.rcpt_no;
				}
				else if(!calculatorMode)
				{
					rec.rec.rcpt_no = mc.mc.rcpt_no;
					rec.rec.kot_no = rec.rec.rcpt_no;
				}

				for(i = 0; i < RECDAT_LEN; i++)
					g_8K_membuf[dataCount++] = rec.arr[i];

				selectedProd++;
				recCountLoop++;
			}
			if(dataCount > TEMP_REC_PUSH_ARRAY)
			{
				if(mc.mc.currentPG)
					FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
				else
					FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
				endAdd += recCountLoop;
				recCountLoop = 0;
				dataCount = 0;
			}
			else if((dataCount <= TEMP_REC_PUSH_ARRAY) && (count == (recArrayPtr - 1)))
			{
				if(mc.mc.currentPG)
					FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
				else
					FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
				endAdd += recCountLoop;
				recCountLoop = 0;
				dataCount = 0;
			}
		}

		if(mc.mc.currentPG)
			mc.mc.recs = endAdd;
		else
			mc.mc.rec1 = endAdd;
		store_mc();
	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	}

	if(mc.mc.udf_en_dis && (!calculatorMode))
	{
		if((print_mode == 1) || (print_mode == 3))
		{
			if(mc.mc.kot_no > MAX_RCPTNO)
				mc.mc.kot_no = 1;
			else
				mc.mc.kot_no++;
		}
	}
}
//-------------------------------------------------------------------------
void KOTStatus(void)
{
	unsigned int i = 0, kotOpen = 0;
	unsigned char exit = 0, key = 0, colCount = 0;
	unsigned char xpos = 1,ypos = 1,count = 0;
	if(mc.mc.udf_en_dis)
	{
		for(i = 1; i <= mc.mc.max_table; i++)
		{
			get_udf(i);

			if(udfno.udfno.udfActive == 1)
			{
				kotOpen++;
			}
		}
	}
	else
	{
		for(i = 1; i < 250; i++)
		{
			get_udf(i);

			if((udfno.udfno.udfActive == 1) && (udfno.udfno.udfActive != 255))
			{
				kotOpen++;
			}
		}
	}

	if(kotOpen)
	{
		while(!exit)
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(1,1);
			lcdprint("KOT's AVAILABLE : ");
			double_height = 0;
			double_width = 0;
			gotoxy(3,1);
			lcdprint("1. KEEP           ");
			gotoxy(4,1);
			lcdprint("2. CLEAR          ");
			gotoxy(5,1);
			lcdprint("3. PRINT DETAILS  ");
			gotoxy(6,1);
			lcdprint("4. DISPLAY DETAILS");
			

			while(!(key = keyPressCheck()));

			switch(key)
			{
				case '1':
							exit = 1;
							udf_no = 0;
							break;
				case '2':
							if(managerLogin)
							{
//								clearKOTs();
								if(yes_no("  CLEAR ALL KOT's?  "))
									clearAllKOT();
								exit = 1;
							}
							else
							{
								clrscr();
								double_height = 1;
								double_width = 0;
								gotoxy(3,1);
								lcdprint("    ONLY MANAGER    ");
								gotoxy(5,1);
								lcdprint("     CAN ACCESS     ");
								beep(SHRTBEEP);
								Delay_ms(2000);
							}
							break;
				case '3':
							disp_printmsg();
							setLanguage(1);
							prt_bar();
							unicode_prt_buf("           AVAILABLE KOT's FOR TABLES           ");
							unicode_print(DH);
							prt_bar();
							for(i = 1 , colCount = 0; i < 255; i++)
							{
								get_udf(i);
								get_table_map(i); //vikas for tablemap
								if(udfno.udfno.udfActive == 1)
								{
									hex_asc(tablemap.tablemap.table_map, asc , 4);
									//hex_asc(i, asc, 3);
									//rem_zeros(asc, 2);
									rem_zeros(asc ,3);
									unicode_prnbuf(asc[0]);
									unicode_prnbuf(asc[1]);
									unicode_prnbuf(asc[2]);
									unicode_prnbuf(asc[3]); //vikas
									unicode_prnbuf(' ');

									colCount += 5;
									if(colCount == 45) //vikas 5*9
									{
										colCount = 0;
										unicode_print(CC);
									}
								}
							}
							unicode_print(CC);
							prt_bar();
							paper_feed();
							exit = 1;
							break;
				case '4':
							clrscr();
							double_width = 0;
							double_height = 0;
							xpos = 1;
							ypos = 1;
							count = 0;
							gotoxy(xpos++,ypos);
							lcdprint("   AVAILABLE KOT's  ");
							for(i = 1 , colCount = 0; i < 255; i++)
							{
								get_udf(i);
								get_table_map(i);
								if(udfno.udfno.udfActive == 1)
								{
									hex_asc(tablemap.tablemap.table_map, asc , 4);
									gotoxy(xpos,ypos);
									rem_zeros(asc ,3);
									disp_char(asc[0]);
									disp_char(asc[1]);
									disp_char(asc[2]);
									disp_char(asc[3]);
									disp_char(' ');

									colCount += 5;
									ypos += 5;
									if(colCount == 20)
									{
										colCount = 0;
										xpos++;
										ypos = 1;
										count++;
										if(count > 6 || xpos > 7)
											break;
									}
								}
							}
							exit = 1;
							gotoxy(8,1);
							lcdprint("   PRESS ANY KEY ...");
							while(!(key = keyPressCheck()));
							break;
				default:
							break;
			}

		}
	}
}
//-------------------------------------------------------------------------
void clearAllKOT(void)
{
	volatile UCHAR tableArr[255] = {0}, tableArrPtr = 0, tableNo = 0;
	UINT32 RECORD_START_ADD = 0, loopProdSumm = 0, loop = 0, loopCount = 0, totalRecs = 0, recs = 0, reccount = 0;
	UCHAR found = 0, i = 0;
	UINT32 endAdd = 0, dataCount = 0;
	UINT16 recCountLoop = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(3,1);
	lcdprint("    CLEARING KOT    ");

	for(i = 0, tableArrPtr = 0; i < mc.mc.max_table; i++)
	{
		get_udf(i);
		if(udfno.udfno.udfActive)
		{
			tableArr[tableArrPtr++] = i;
		}
	}

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	recCountLoop = 0;
	dataCount = 0;

	endAdd = recs;

	for(tableNo = 0; tableNo < tableArrPtr; tableNo++)
	{
		read_rtc();
		for(i = 0; i < RECDAT_LEN; i++)
			rec.arr[i] = 0;

		rec.rec.strt_byt = 'L';
	    rec.rec.dd = tim.tim.dd;
	    rec.rec.mm = tim.tim.mm;
	    rec.rec.yy = tim.tim.yy;
	    rec.rec.hr = tim.tim.hr;
	    rec.rec.mn = tim.tim.mn;
	    rec.rec.sc = tim.tim.sc;

		rec.rec.product_code = 0;
	    rec.rec.rate = 0;
		rec.rec.quantity = 0;
		rec.rec.unit_type = 0;

		rec.rec.payment_mode = paymentType;

		//store operator
		rec.rec.agent_no = operatorCode;

		if(mc.mc.udf_en_dis)
			rec.rec.rcpt_no = udfno.udfno.udfRcptNo;
		else
			rec.rec.rcpt_no	= mc.mc.rcpt_no;

		lastRecReceiptNo = rec.rec.rcpt_no;

		rec.rec.final_prod_amount = 0;
		rec.rec.discount_amt = 0;
		rec.rec.sgst_amt = 0;
		rec.rec.cgst_amt = 0;
		rec.rec.total_amt = 0;
		rec.rec.other_chrg = 0;
		rec.rec.other_chrg_rate = 0;
		rec.rec.round_off_tot_amt = 0;

		rec.rec.bill_stat = 'D';
		rec.rec.calc_stat = 0;

		get_udf(tableArr[tableNo]);
		rec.rec.udf_no = tableArr[tableNo];
		rec.rec.rcpt_no = udfno.udfno.udfRcptNo;

		rec.rec.round_off_tot_amt = 0;
		rec.rec.last = 1;

		for(i = 0; i < RECDAT_LEN; i++)
			g_8K_membuf[dataCount++] = rec.arr[i];

		recCountLoop++;

		if(dataCount > TEMP_REC_PUSH_ARRAY)
		{
			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
			endAdd += recCountLoop;
			recCountLoop = 0;
			dataCount = 0;
		}
		else if((dataCount <= TEMP_REC_PUSH_ARRAY) && (tableNo == (tableArrPtr - 1)))
		{
			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(endAdd * RECDAT_LEN))), g_8K_membuf, dataCount);
			endAdd += recCountLoop;
			recCountLoop = 0;
			dataCount = 0;
		}
	}

	if(mc.mc.currentPG)
		mc.mc.recs = endAdd;
	else
		mc.mc.rec1 = endAdd;
	store_mc();

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;


	totalRecs = (recs / TEMP_RECS_DISC_ARRAY);
	if((recs % TEMP_RECS_DISC_ARRAY) > 0)
		totalRecs += 1;

	RECORD_START_ADD = 1;
	for(loopProdSumm = 0; loopProdSumm < totalRecs; loopProdSumm++)
	{
		reccount = loadRecordsinTempArray(RECORD_START_ADD);

		if(reccount > recs)
			break;

		found = 0;
		for(loop = RECORD_START_ADD; (loop < recs) && (loop < ((RECORD_START_ADD + TEMP_RECS_DISC_ARRAY))); loop++)
		{
			for(i = 0; i < RECDAT_LEN; i++)
				rec.arr[i] = save_rec[loop - RECORD_START_ADD].arr[i];

			if(rec.rec.bill_stat == 'O')
			{
				found = 1;
				rec.rec.bill_stat = 'D';
				rec.rec.strt_byt = 'D';
			}

			for(i = 0; i < RECDAT_LEN; i++)
				save_rec[loop - RECORD_START_ADD].arr[i] = rec.arr[i];
		}
		if(found)
		{
			for(loop = 0, loopCount = 0; (loop < TEMP_RECS_DISC_ARRAY); loop++)
			{
				for(i = 0; i < RECDAT_LEN; i++)
					g_8K_membuf[loopCount++] = save_rec[loop].arr[i];
			}

			if(mc.mc.currentPG)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
		}

	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

		RECORD_START_ADD += TEMP_RECS_DISC_ARRAY;
	}

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

	for(i = 0; i < 2; i++)
	{
		FlashModifyData((((unsigned long)(UDF_PRD_CNT_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
	}

	for(i = 0; i < 9; i++)
	{
		FlashModifyData((((unsigned long)(UDF_PG * PGBYTS)) + ((unsigned long)(i * 255))), g_8K_membuf, 255);
	}
	gotoxy(3,1);
	lcdprint("     KOT CLEARED    ");
	beep(LONGBEEP);
	Delay_ms(1000);
}
//-------------------------------------------------------------------------
void login(void)
{
	UCHAR psdBuff[AGENT_PASSWORD_LEN], userID[AGENT_ID_LEN],psdBuff1[AGENT_PASSWORD_LEN], userID1[AGENT_ID_LEN];
	UINT16 loopCount = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	loginProcess = 1;

	gotoxy(1,1);
	lcdprint("        LOGIN       ");
	double_height = 1;
	double_width = 0;

	spaceFlag = 1;

	gotoxy(3,1);
	lcdprint("USER ID  : ");
	memset(userID, ' ', AGENT_ID_LEN);
	if(getAlphaDig(3,14,userID,AGENT_ID_LEN, 1, 0))
	{
		memset(membuf, ' ', AGENT_TOTAL_LEN);
		for(loopCount = 1; membuf[0] != END_CHR; loopCount++)
		{
			memset(userID1, ' ', AGENT_ID_LEN);
			memset(psdBuff1, ' ', AGENT_PASSWORD_LEN);

			FlashReadData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) loopCount * AGENT_TOTAL_LEN)) + AGENT_PASSWORD, psdBuff1, AGENT_PASSWORD_LEN);
			FlashReadData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) loopCount * AGENT_TOTAL_LEN)) + AGENT_ID, userID1, AGENT_ID_LEN);
			FlashReadData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) loopCount * AGENT_TOTAL_LEN)), membuf, AGENT_TOTAL_LEN);

			if((!strncmp(userID, userID1, AGENT_ID_LEN)))
			{
				if((asc_hex(&membuf[AGENT_FLAG], AGENT_FLAG_LEN) == 1) && (loopCount == 1))
				{
					double_height = 0;
					double_width = 0;
					gotoxy(7,1);
					lcdprint("             MANAGER");
				}
				else
				{
					double_height = 0;
					double_width = 0;
					gotoxy(7,1);
					lcdprint("                    ");
				}
			}
		}
		double_height = 1;
		double_width = 0;
		gotoxy(5,1);
		lcdprint("PASSWORD : ");
		if(getAlphaDig(5,14, psdBuff,AGENT_PASSWORD_LEN, 1, 1))
		{
			if((!strncmp(userID, "1000", AGENT_ID_LEN)))
			{
				if((!strncmp(psdBuff,"193728", AGENT_PASSWORD_LEN)))
				{
					changeManagerPassword(0);
					clrscr();
					double_height = 1;
					double_width = 0;
					gotoxy(3,1);
					lcdprint("   PLEASE RESTART   ");
					gotoxy(5,1);
					lcdprint("       MACHINE      ");
					while(1)
					{
						while(!(keyPressCheck()));
					}
				}
			}

			memset(membuf, ' ', AGENT_TOTAL_LEN);
			for(loopCount = 1; membuf[0] != END_CHR; loopCount++)
			{
				memset(userID1, ' ', AGENT_ID_LEN);
				memset(psdBuff1, ' ', AGENT_PASSWORD_LEN);

				FlashReadData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) loopCount * AGENT_TOTAL_LEN)) + AGENT_PASSWORD, psdBuff1, AGENT_PASSWORD_LEN);
				FlashReadData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) loopCount * AGENT_TOTAL_LEN)) + AGENT_ID, userID1, AGENT_ID_LEN);
				FlashReadData((((UINT32) AGENT_PG * PGBYTS) + ((UINT32) loopCount * AGENT_TOTAL_LEN)), membuf, AGENT_TOTAL_LEN);

				if((!strncmp(userID, userID1, AGENT_ID_LEN)))
				{
					if((!strncmp(psdBuff,psdBuff1, AGENT_PASSWORD_LEN)))
					{
						loginFlag = 1;

						if((asc_hex(&membuf[AGENT_FLAG], AGENT_FLAG_LEN) == 1) && (loopCount == 1))
						{
							managerLogin = 1;
							beep(LONGBEEP);
						}
						else
							managerLogin = 0;
						operatorCode = asc_hex(&membuf[AGENT_CODE], AGENT_CODE_LEN);
						operatorPos = loopCount;
						clrscr();
						double_height = 1;

						gotoxy(3,1);
						lcdprint("  LOGIN SUCCESSFUL  ");
						
						loginProcess = 0;
						beep(OKKKBEEP);
						Delay_ms(2000);
						clrscr();

						if(mc.mc.udf_en_dis)
							KOTStatus();

						clrscr();

						return;
					}
				}
			}
		}
	}
	double_height = 1;
	double_width = 0;
	clrscr();
	gotoxy(3,1);
	lcdprint("    LOGIN FAILED    ");
	beep(ERRRBEEP);
	Delay_ms(3000);

}
//-------------------------------------------------------------------------
void calculator_mode(void)
{
	UCHAR exit = 0, i = 0, entry = 0, entryCount = 0, count = 0;
	UINT32 temp_qty = 0, temp_rate = 0, loopStart = 0;
	UINT64 temp_tot_amount = 0;

	clear_global_variable();
	recArrayPtr = 0;

	while(!exit)
	{
		calculatorMode = 1;
		clrscr();

		double_height = 0;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("   CALCULATOR MODE  ");
		
		gotoxy(8,1);
		lcdprint("PRESS          v/ENT");
		double_height = 1;
		double_width = 0;

	
		gotoxy(3,1);
		lcdprint("QTY  : ");
		gotoxy(5,1);
		lcdprint("TOTAL: ");
		
		gotoxy(5,8);
		if(temp_tot_amount > 0)
		{
			value_hex_asc_64U = temp_tot_amount;
			hex_asc_64U(asc, 10);
			rem_zeros(asc, 8);
			for(i = 0; i < 10; i++)
			{
				if(i == 8)
					disp_char('.');
				disp_char(asc[i]);
			}
		}
		if (selectedProd > 0)
		{
			double_height = 0;
			double_width = 0;
			gotoxy(7,1);
			lcdprint("COUNT: ");
			hex_asc(selectedProd, asc, 3);
			rem_zeros(asc, 3);
			for(count = 0; count < 3; count++)
			{
				disp_char(asc[count]);
			}
		}
		double_height = 1;
		if(mc.mc.qty_enter_method == QTY_4_2)
			temp_qty = get_dig_dot_calc(3, 8, mc.mc.digitsBeforeDot, mc.mc.digitsAfterDot, 100, 1);
		else
			temp_qty = get_dig_dot_calc(3, 8, mc.mc.digitsBeforeDot, mc.mc.digitsAfterDot, 1000, 1);
		if((temp_qty > 0) && (!get_dig_dot_esc_key))
		{
			entry++;

			qty = temp_qty;
			double_height = 1;
			gotoxy(3,1);
			lcdprint("RATE : ");
			double_height = 0;
			double_width = 0;
			gotoxy(5,1);
			lcdprint ("                    ");
			gotoxy (6, 1);
			lcdprint ("                    ");
			gotoxy (7, 1);
			lcdprint("                    ");

			double_height = 1;
			if(entryCount < (MAX_TEMP_RECORDS + 1))
			{
				if((temp_rate = get_dig_dot(3, 8, 6, 2, 0, 1)))
				{
					final_bill_amount = temp_rate;

					if(store_calc())
					{
						entryCount++;
						temp_tot_amount += calculate_rate();
					}
				}
			}
			else
			{
				clrscr();
				double_height = 1;
				double_width = 0;
				gotoxy(3,1);
				lcdprint("  MAX ITEMS REACHED ");
				gotoxy(5,1);
				lcdprint("  PLEASE PRINT BILL ");
				beep(ERRRBEEP);
				Delay_ms(2000);
			}
		}
		else if((!temp_qty) && (get_dig_dot_prn_key))
		{
			if(entryCount > 0)
			{
				saveOrder();
				if(store_calc_last())
				{
					calculator_print();

					if(mc.mc.drawer_en_dis) //VIKAS
					{
						DRW_ENABLE = 1;
						Delay_ms(200);
						DRW_ENABLE = 0;
					}

					udf_no = 0;
					selectedProd = 0;
					selectedAmount = 0;

					mc.mc.rec_start = 0;
					mc.mc.selected = 0;
					recArrayPtr = 0;

					if(mc.mc.calculatorBillNo >= MAX_RCPTNO)
						mc.mc.calculatorBillNo = 1;
					else
						mc.mc.calculatorBillNo++;

					store_mc();

//					if(mc.mc.gprs_en_dis == RECORD_LOT)
//					{
//						if(mc.mc.gprs_record_sent >= mc.mc.gprs_recordsLot)
//						{
//							sendDataGPRS(1);
//						}
//					}
//
//					if(mc.mc.gsm_en_dis == RECORD_LOT)
//					{
//						if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
//						{
//							sendSMSDayWiseSummary();
//						}
//					}

					clear_global_variable();
					recArrayPtr = 0;
					temp_qty = 0;
					temp_rate = 0;
					temp_tot_amount = 0;
					entry = 0;
					entryCount = 0;
				}
			}
		}
		else if((entry) && (get_dig_dot_esc_key))
		{
			if(entryCount > 0)
			{
				if(yes_no("     EDIT BILL?     "))
				{
					editCalculatorOrder();
					temp_tot_amount = 0;
					entryCount = 0;
					for(loopStart = 0;loopStart < recArrayPtr; loopStart++)
					{
						for(i = 0; i < RECDAT_LEN; i++)
						{
							temprec.arr[i] = recArray[loopStart][i];
						}
						if(temprec.temprec.bill_stat == 'O')
						{
							temp_tot_amount += temprec.temprec.final_prod_amount;
							entryCount++;
						}
					}
				}
				else
					entry = 0;
				qty = 0;
				final_bill_amount = 0;
			}
			else
			{
				exit = 1;
				udf_no = 0;
				selectedProd = 0;
				mc.mc.rec_start = 0;
				mc.mc.selected = 0;
				selectedQty = 0;
				selectedAmount = 0;
				store_mc();
				recArrayPtr = 0;
			}
			qty = 0;
			final_bill_amount = 0;
		}
		else if((!entry) && (get_dig_dot_esc_key))
		{
			if(yes_no("        EXIT?       "))
			{
				exit = 1;
				udf_no = 0;
				selectedProd = 0;
				mc.mc.rec_start = 0;
				mc.mc.selected = 0;
				selectedAmount = 0;
				selectedQty = 0;
				store_mc();
				recArrayPtr = 0;
			}
			else
			{
				entry = 1;
			}
		}
	}
}
//-------------------------------------------------------------------------
void calculator_mode_swapped(void)
{
	UCHAR exit = 0, i = 0, entry = 0, entryCount = 0;
	UINT32 temp_qty = 0, temp_rate = 0, loopStart = 0;
	UINT64 temp_tot_amount = 0;
	UCHAR count = 0;

	clear_global_variable();
	recArrayPtr = 0;

	while(!exit)
	{
		calculatorMode = 1;
		clrscr();

		double_height = 0;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("   CALCULATOR MODE  ");

		gotoxy(8,1);
		lcdprint("PRESS          v/ENT");
		double_height = 1;
		double_width = 0;


		gotoxy(3,1);
		lcdprint("RATE : ");

		gotoxy(5,1);
		lcdprint("TOTAL: ");

		gotoxy(5,8);
		if(temp_tot_amount > 0)
		{
			value_hex_asc_64U = temp_tot_amount;
			hex_asc_64U(asc, 10);
			rem_zeros(asc, 8);
			for(i = 0; i < 10; i++)
			{
				if(i == 8)
					disp_char('.');
				disp_char(asc[i]);
			}
		}
		if (selectedProd > 0)
		{
			double_height = 0;
			double_width = 0;
			gotoxy(7,1);

			lcdprint("COUNT: ");
			hex_asc(selectedProd, asc, 3);
			rem_zeros(asc, 3);
			for(count = 0; count < 3; count++)
			{
				disp_char(asc[count]);
			}
		}
		double_height = 1;
		temp_rate = get_dig_dot_calc(3, 8, 6, 2, 0, 1);
		if((temp_rate > 0) && (!get_dig_dot_esc_key))
		{
			clrRow(3);
			entry++;
			final_bill_amount = temp_rate;
			double_height = 1;
			gotoxy(3,1);
			lcdprint("QTY  :   ");
			
			double_height = 0;
			double_width = 0;
			gotoxy(5,1);
			lcdprint("                    ");
			gotoxy (6, 1);
			lcdprint ("                    ");
			gotoxy (7, 1);
			lcdprint("                    ");

			double_height = 1;
			if(entryCount < (MAX_TEMP_RECORDS + 1))
			{
				if(mc.mc.qty_enter_method == QTY_4_2)
					temp_qty = get_dig_dot_calc(3, 8, mc.mc.digitsBeforeDot, mc.mc.digitsAfterDot, 100, 1);
				else
					temp_qty = get_dig_dot_calc(3, 8, mc.mc.digitsBeforeDot, mc.mc.digitsAfterDot, 1000, 1);
				if((temp_qty > 0) && (!get_dig_dot_esc_key))
				{
					qty = temp_qty;

					if(store_calc())
					{
						entryCount++;
						temp_tot_amount += calculate_rate();
					}
				}
            }
            else
			{
				clrscr();
				double_height = 1;
				double_width = 0;
				gotoxy(3,1);
				lcdprint("  MAX ITEMS REACHED ");
				gotoxy(5,1);
				lcdprint("  PLEASE PRINT BILL ");
				beep(ERRRBEEP);
				Delay_ms(2000);
			}
		}
		else if((!temp_rate) && (get_dig_dot_prn_key))
		{
			if(entryCount > 0)
			{
				saveOrder();
				if(store_calc_last())
				{
					calculator_print();

					if(mc.mc.drawer_en_dis) //VIKAS
					{
						DRW_ENABLE = 1;
						Delay_ms(200);
						DRW_ENABLE = 0;
					}

					udf_no = 0;
					selectedProd = 0;
					selectedAmount = 0;

					mc.mc.rec_start = 0;
					mc.mc.selected = 0;
					recArrayPtr = 0;

					if(mc.mc.calculatorBillNo >= MAX_RCPTNO)
						mc.mc.calculatorBillNo = 1;
					else
						mc.mc.calculatorBillNo++;

					store_mc();

					if(mc.mc.gprs_en_dis)
					{
						stopPostCMDToModem();
						pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
					}
					if(mc.mc.gsm_en_dis == RECORD_LOT)
					{
						if(mc.mc.gsm_record_sent >= mc.mc.gsm_recordsLot)
						{
							sendDayWiseSummary_SMSToAllMobileNumbers();
						}
					}

					clear_global_variable();
					recArrayPtr = 0;
					temp_qty = 0;
					temp_rate = 0;
					temp_tot_amount = 0;
					entry = 0;
					entryCount = 0;
				}
			}
		}
		else if((entry) && (get_dig_dot_esc_key))
		{
			if(entryCount > 0)
			{
				if(yes_no("     EDIT BILL?     "))
				{
					editCalculatorOrder();
					temp_tot_amount = 0;
					entryCount = 0;
					for(loopStart = 0;loopStart < recArrayPtr; loopStart++)
					{
						for(i = 0; i < RECDAT_LEN; i++)
						{
							temprec.arr[i] = recArray[loopStart][i];
						}
						if(temprec.temprec.bill_stat == 'O')
						{
							temp_tot_amount += temprec.temprec.final_prod_amount;
							entryCount++;
						}
					}
				}
				else
					entry = 0;
				qty = 0;
				final_bill_amount = 0;
			}
			else
			{
				exit = 1;
				udf_no = 0;
				selectedProd = 0;
				mc.mc.rec_start = 0;
				mc.mc.selected = 0;
				selectedQty = 0;
				selectedAmount = 0;
				store_mc();
				recArrayPtr = 0;
			}
			qty = 0;
			final_bill_amount = 0;
		}
		else if((!entry) && (get_dig_dot_esc_key))
		{
			if(yes_no("        EXIT?       "))
			{
				exit = 1;
				udf_no = 0;
				selectedProd = 0;
				mc.mc.rec_start = 0;
				mc.mc.selected = 0;
				selectedAmount = 0;
				selectedQty = 0;
				store_mc();
				recArrayPtr = 0;
			}
			else
			{
				entry = 1;
			}
		}
	}
}
//-------------------------------------------------------------------------
void editCalculatorOrder(void)
{
	UCHAR exitMain = 0, exit = 0, count = 0, unit_func = 0, i = 0, key = 0, entry = 0, BKloopStart = 0;
	UINT16 j = 0;
	UINT32 loopRecs = 0, loopStart = 0, dispRec = 0, startRec = 0;

	exit = 0;
	while(!exitMain)
	{
		for(loopStart = 0; loopStart < recArrayPtr; )
		{
			for(i = 0; i < RECDAT_LEN; i++)
			{
				temprec.arr[i] = recArray[loopStart][i];
			}
			entry = 0;

			if(temprec.temprec.bill_stat == 'O')
				entry = 1;

			if(entry)
			{
				dispRec = loopStart;

				clrscr();
				double_height = 1;
				double_width = 0;

				gotoxy(1,1);
				lcdprint("CALCULATOR BILL EDIT");

				gotoxy(3,1);
				disp_char('Q');
				disp_char('T');
				disp_char('Y');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				hex_asc(temprec.temprec.quantity, asc, 6);
				rem_zeros(asc,2);
				for(i = 0;i < 6;i++)
				{
					if(i == 3)
						disp_char('.');
					disp_char(asc[i]);
				}

				gotoxy(5,1);
				disp_char('R');
				disp_char('A');
				disp_char('T');
				disp_char('E');
				disp_char(' ');
				disp_char(':');
				disp_char(' ');
				hex_asc(temprec.temprec.rate, asc, 8);
				rem_zeros(asc,4);
				for(i = 0; i < 8; i++)
				{
					if(i == 6)
						disp_char('.');
					disp_char(asc[i]);
				}

				double_height = 0;
				double_width = 0;

				gotoxy(8,1);
				lcdprint("PRESS   ENTER/CANCEL");
				while(!(key = keyPressCheck()));

				switch(key)
				{
				case UP_ARROW:
									if(dispRec > 0)
									{
										exit = 0;
										loopStart--;

										if(loopStart < startRec)
											loopStart = 0;
										while(loopStart > startRec && loopStart < recArrayPtr)
										{
											for(i=0 ; i < RECDAT_LEN ; i++)
											{
												temprec.arr[i] = recArray[loopStart][i];
											}
											if(temprec.temprec.bill_stat == 'D')
											{
												loopStart--;
											}
											else
											{
												break;
											}
										}
										if(loopStart < startRec)
											loopStart = 0;
										dispRec = loopStart;
									}
									break;
					case DOWN_ARROW:
									if(dispRec < (recArrayPtr - 1))
									{
										exit = 0;
										BKloopStart = loopStart;
										loopStart++;
										while(loopStart < recArrayPtr )
										{
											for(i = 0 ; i < RECDAT_LEN ; i++)
											{
												temprec.arr[i] = recArray[loopStart][i];
											}
											if(temprec.temprec.bill_stat == 'D')
											{
												loopStart++;
											}
											else
											{
												break;
											}
										}
										if(temprec.temprec.bill_stat == 'D')
										{
											loopStart = BKloopStart;
										}
									}
									break;
					case '*':
									exitMain = 1;
									return;
					case '#':
									customizeSelectedCalculatorOrder(dispRec);
									selectedAmount = 0;
									selectedQty = 0;
									selectedProd = 0;
									for(j = 0; j < recArrayPtr; j++)
									{
										for(i = 0; i < RECDAT_LEN; i++)
										{
											temprec.arr[i] = recArray[j][i];
										}
										if(temprec.temprec.bill_stat == 'O')
										{
											selectedAmount += temprec.temprec.final_prod_amount;

											if(!mc.mc.reverse_tax_en_dis)		//Mrugnil 15/04/25
											{
												selectedAmount += temprec.temprec.sgst_amt;		//Mrugnil 15/04/25
												selectedAmount += temprec.temprec.cgst_amt;		//Mrugnil 15/04/25
											}
											if(temprec.temprec.discount_amt)		//Mrugnil 15/04/25
												selectedAmount -= temprec.temprec.discount_amt;		//Mrugnil 15/04/25

											selectedQty += temprec.temprec.quantity;
											selectedProd++;
										}
									}
									while(loopStart > startRec && loopStart < recArrayPtr)
									{
										for(i=0 ; i < RECDAT_LEN ; i++)
										{
											temprec.arr[i] = recArray[loopStart][i];
										}
										if(temprec.temprec.bill_stat == 'D')
										{
											loopStart--;
										}
										else
										{
											break;
										}
									}
									if(dispRec >= (recArrayPtr - 1))
										loopStart = 0;
									exit = 0;
									if(!exit)
										exitMain = 1;
									break;
					default:
									break;
				}
			}
			else
				loopStart++;
		}
		if(loopStart >= recArrayPtr)
			exitMain = 1;
	}
}
//-------------------------------------------------------------------------
void customizeSelectedCalculatorOrder(unsigned char recPosition)
{
	unsigned char exit1 = 0, stepCount = 0, key = 0, i = 0;
	UINT32 get_qty = 0, get_rate = 0;
	unsigned long long tempvar64U = 0;

	for(i = 0; i < RECDAT_LEN; i++)
		temprec.arr[i] = recArray[recPosition][i];

	while(!exit1)
	{
		clrscr();
		gotoxy(1,1);
		lcdprint("        EDIT        ");
		double_height = 0;
		double_width = 0;
		gotoxy(3,2);
		lcdprint("QTY : ");
		hex_asc(temprec.temprec.quantity, asc, 6);
		rem_zeros(asc,2);
		for(i = 0;i < 6;i++)
		{
			if(i == 3)
				disp_char('.');
			disp_char(asc[i]);
		}

		gotoxy(4,2);
		lcdprint("RATE :  ");
		hex_asc(temprec.temprec.rate, asc, 8);
		rem_zeros(asc,4);
		for(i = 0; i < 8; i++)
		{
			if(i == 6)
				disp_char('.');
			disp_char(asc[i]);
		}
		gotoxy(5,1);
		lcdprint(" TOT :");
		value_hex_asc_64U = temprec.temprec.final_prod_amount;
		hex_asc_64U(asc, 11);
		rem_zeros(asc,7);
		for(i = 0; i < 11; i++)
		{
			if(i == 9)
				disp_char('.');
			disp_char(asc[i]);
		}

		gotoxy(6,2);
		lcdprint("SAVE             ");
		gotoxy(7,2);
		lcdprint("DELETE FROM ORDER");


		switch(stepCount)
		{
			case 0:
					gotoxy(3,1);
					disp_char('>');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(6,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					break;
			case 1:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char('>');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(6,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char(' ');
					break;
			case 2:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(6,1);
					disp_char('>');
					gotoxy(7,1);
					disp_char(' ');
					break;
			case 3:
					gotoxy(3,1);
					disp_char(' ');
					gotoxy(4,1);
					disp_char(' ');
					gotoxy(5,1);
					disp_char(' ');
					gotoxy(6,1);
					disp_char(' ');
					gotoxy(7,1);
					disp_char('>');
					break;
		}

		while(!(key = keyPressCheck()));

		switch(key)
		{
			case UP_ARROW:
							if(stepCount > 0)
								stepCount--;
							break;
			case DOWN_ARROW:
							if(stepCount < 3)
								stepCount++;
							break;
			case '*':
							exit1 = 1;
							break;
			case '#':
							switch(stepCount)
							{
								case 0:
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(1,1);
										lcdprint("   CALCULATOR MODE  ");
										gotoxy(4,1);
										lcdprint("QTY : ");
										get_qty = get_dig_dot(4, 8, 3, 3, temprec.temprec.quantity, 1);
										if(get_qty)
										{
											temprec.temprec.quantity = get_qty;
											tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
//											temprec.temprec.final_prod_amount = (tempvar64U / 1000);
											temprec.temprec.final_prod_amount = (tempvar64U);
										}
										break;
								case 1:
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(1,1);
										lcdprint("   CALCULATOR MODE  ");
										gotoxy(4,1);
										lcdprint("RATE: ");
										get_rate = get_dig_dot(4, 8, 4, 2, temprec.temprec.rate, 1);
										if(get_rate)
										{
											temprec.temprec.rate = get_rate;
											tempvar64U = mult64_32_32(temprec.temprec.quantity, temprec.temprec.rate);
//											temprec.temprec.final_prod_amount = (tempvar64U / 1000);
											temprec.temprec.final_prod_amount = (tempvar64U);
										}
										break;
								case 2:
										for(i = 0; i < RECDAT_LEN; i++)
											recArray[recPosition][i] = temprec.arr[i];
										exit1 = 1;
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("UPDATED SUCCESSFULLY");
										beep(OKKKBEEP);
										Delay_ms(2000);
										break;
								case 3:
										temprec.temprec.bill_stat = 'D';
										temprec.temprec.strt_byt = 'D';
										for(i = 0; i < RECDAT_LEN; i++)
											recArray[recPosition][i] = temprec.arr[i];
										exit1 = 1;
										clrscr();
										double_height = 1;
										double_width = 0;
										gotoxy(3,1);
										lcdprint("REMOVED SUCCESSFULLY");
										beep(OKKKBEEP);
										Delay_ms(2000);
										break;
							}
							break;
			default:
							break;
		}
	}
}
//-------------------------------------------------------------------------
unsigned char store_calc(void)
{
	UCHAR i = 0;

	//store date time
	read_rtc();
	rec.rec.strt_byt = 'S';
    rec.rec.dd = tim.tim.dd;
    rec.rec.mm = tim.tim.mm;
    rec.rec.yy = tim.tim.yy;
    rec.rec.hr = tim.tim.hr;
    rec.rec.mn = tim.tim.mn;
    rec.rec.sc = tim.tim.sc;

	rec.rec.calc_stat = 'C';
	rec.rec.bill_stat = 'O';

    rec.rec.rate = final_bill_amount;
	rec.rec.quantity = qty;
    rec.rec.final_prod_amount = calculate_rate();
	rec.rec.total_amt = 0;
	rec.rec.agent_no = operatorCode;
	rec.rec.last = 0;
	rec.rec.payment_mode = 1;

	if(rec.rec.final_prod_amount)
	{
		if(!mc.mc.rec_start)
		{
			if(mc.mc.currentPG)
				mc.mc.rec_start = mc.mc.recs;
			else
				mc.mc.rec_start = mc.mc.rec1;
		}
		rec.rec.rcpt_no = mc.mc.calculatorBillNo;


		mc.mc.selected++;

		for(i = 0; i < RECDAT_LEN; i++)
		{
			recArray[recArrayPtr][i] = rec.arr[i];
		}
		recArrayPtr++;
		selectedQty += rec.rec.quantity;

		if(!startProd)
		{
			if(mc.mc.currentPG)
				startProd = mc.mc.recs;
			else
				startProd = mc.mc.rec1;
		}
		selectedProd++;
		return 1;
	}
	return 0;
	qty = 0;
	final_bill_amount = 0;
}
//-------------------------------------------------------------------------
unsigned char store_calc_last(void)
{
	UINT32 loop = 0;
	UINT64 final_amt = 0;
	UINT64 final_prod_amount = 0, final_tot_amount = 0;
	UCHAR i = 0;
	UINT16 cust_det = 0;

	if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_CUST_NAME))
	{
		clrscr();
		double_height = 1;
		double_width = 0;
		gotoxy(1,1);
		lcdprint("ENTER CUSTOMER NAME ");
		double_height = 0;
		gotoxy(8,1);
		lcdprint("PRESS        ENT/CNL");
		double_height = 1;
		memset(g_customerName, ' ',CUSTOMER_NAME_LEN);
		getAlphaDig(4, 1, g_customerName, CUSTOMER_NAME_LEN, 1, 0);
	}

//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint("   SAVING RECORDS   ");

	//store date time
	read_rtc();
	rec.rec.total_amt = 0;

	startRec = mc.mc.rec_start;
	if(mc.mc.currentPG)
		endRec = mc.mc.recs;
	else
		endRec = mc.mc.rec1;

	closeOpenBill(startRec, endRec, 1);

	selectedQty = 0;
	final_prod_amount = 0;
	for(loop = startRec, i = 0; loop < endRec; loop++)
	{
		get_rec(loop);

		if((rec.rec.final_prod_amount) && (rec.rec.calc_stat == 'C'))
		{
			final_amt += rec.rec.final_prod_amount;
			selectedQty += rec.rec.quantity;
		}
	}

	if(final_amt)
		final_prod_amount = final_amt;
	else
		return 0;

	final_tot_amount = final_prod_amount;

	final_amt = 0;

	for(i = 0; i < RECDAT_LEN; i++)
		rec.arr[i] = 0;

	rec.rec.strt_byt = 'L';
    rec.rec.dd = tim.tim.dd;
    rec.rec.mm = tim.tim.mm;
    rec.rec.yy = tim.tim.yy;
    rec.rec.hr = tim.tim.hr;
    rec.rec.mn = tim.tim.mn;
    rec.rec.sc = tim.tim.sc;

	rec.rec.product_code = 0;
    rec.rec.rate = 0;
	rec.rec.quantity = selectedQty;
	rec.rec.unit_type = 0;

	//store operator
	rec.rec.agent_no = operatorCode;
	rec.rec.rcpt_no	= mc.mc.calculatorBillNo;

	rec.rec.final_prod_amount = final_prod_amount;
	rec.rec.discount_amt = 0;
	rec.rec.sgst_amt = 0;
	rec.rec.total_amt = final_tot_amount;
	rec.rec.cgst_amt = 0;
	rec.rec.round_off_tot_amt = rec.rec.total_amt;

	rec.rec.calc_stat = 'C';
	rec.rec.bill_stat = 'C';

	rec.rec.udf_no = 0;
	rec.rec.slab_no = 0;
	rec.rec.payment_mode = 1;

	rec.rec.last = 1;

	if(mc.mc.cust_name_entry)
	{
		if(mc.mc.currentPG)
			cust_det = mc.mc.cust_details0;
		else
			cust_det = mc.mc.cust_details1;

		rec.rec.cust_detail_pos = cust_det;
	}
	else
		rec.rec.cust_detail_pos = 0;

	if(mc.mc.currentPG)
		store_rec(mc.mc.recs);
	else
		store_rec(mc.mc.rec1);

	returnBill = 0;

//	saveSummaryDetails();

	if(mc.mc.currentPG)
		mc.mc.recs++;
	else
		mc.mc.rec1++;

	store_mc();

	if(mc.mc.gprs_en_dis == RECORD_LOT)
		mc.mc.gprs_record_sent++;
	if(mc.mc.gsm_en_dis == RECORD_LOT)
		mc.mc.gsm_record_sent++;

	if(mc.mc.cust_name_entry)
	{
		if(mc.mc.currentPG)
			cust_det = mc.mc.cust_details0;
		else
			cust_det = mc.mc.cust_details1;

		if(cust_det < MAX_CUSTOMER_DETAILS)
		{
			cust_details.cust_details.receipt_no = rec.rec.rcpt_no;
			for(i = 0; i < CUSTOMER_NAME_LEN; i++)
				cust_details.cust_details.CUSTOMER_NAME[i] = g_customerName[i];
			for(i = 0; i < GSTIN_NO_LEN; i++)
				cust_details.cust_details.GSTIN[i] = ' ';

			store_cust_details(cust_det, mc.mc.currentPG);

			cust_det++;

			if(mc.mc.currentPG)
				mc.mc.cust_details0 = cust_det;
			else
				mc.mc.cust_details1 = cust_det;
		}
		else
		{
			clrscr();
			double_height = 1;
			double_width = 0;
			gotoxy(1,1);
			lcdprint("MAX CUSTOMER DETAILS");
			gotoxy(3,1);
			lcdprint("REACHED. CANNOT SAVE");
			gotoxy(5,1);
			if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_CUST_NAME))
			{
				lcdprint("    CUSTOMER NAME   ");
				gotoxy(7,1);
			}
			if((mc.mc.cust_name_entry == CUST_BOTH) || (mc.mc.cust_name_entry == O_GSTIN))
			{
				lcdprint("    GSTIN NUMBER    ");
			}
			beep(ERRRBEEP);
			Delay_ms(2000);
		}
	}

	store_mc();

	return 1;
}
//-------------------------------------------------------------------------
unsigned long get_mins(unsigned char dt, unsigned char mt, unsigned char yr, unsigned char hr, unsigned char mn, unsigned char conversionFlag)
{
	unsigned char i = 0, dd = 0, mm = 0, yy = 0, hh = 0, min = 0;
	unsigned long mins = 0;
	unsigned long days = 0;

	if(conversionFlag)
	{
		tim.tim.dd = dt;
		tim.tim.mm = mt;
		tim.tim.yy = yr;
		tim.tim.hr = hr;
		tim.tim.mn = mn;

		tim_hex_asc();

		dd = asc_hex(dd2, 2);
		mm = asc_hex(mm2, 2);
		yy = asc_hex(yy2, 2);
		hh = asc_hex(hr2, 2);
		min = asc_hex(mn2, 2);
	}
	else
	{
		dd = dt;
		mm = mt;
		yy = yr;
		hh = hr;
		min = mn;
	}

	if(yy)
	{
		days =  (unsigned long)yy * 365;
		days += ((yy - 1) / 4);
	}

	for(i = 1 ; i < mm ; i++)
	{
		switch(i)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
					days += 31;
					break;
			case 4:
			case 6:
			case 9:
			case 11:
					days += 30;
					break;
			case 2:
					if(yy % 4)
						days += 28;
					else
						days += 29;
					break;
		}
	}
	days += (dd - 1);

	mins = (unsigned long)days * 1440;
	mins += ((unsigned long)hh * 60);
	mins += min;

	return mins;
}
//-------------------------------------------------------------------------
//nilesh
//clear the barcode array in RAM
void clear_barcode_details()
{
	int loop = 0;
	for(loop = 0 ; loop < MAX_PRODUCTS ; loop++)
	{
		memset(barcode_details[loop].arr,0,sizeof(barcode_details[loop].arr));
	}
}
//-------------------------------------------------------------------------
//nilesh
//get the barcode details from flash into the RAM
//if position is 0 then all the details will be fetched into RAM
//if position is > 0 then single product details will be fetched.
void get_barcode_details(int position)
{
	if(usb_barcode_enable == TRUE)
	{
		clear_barcode_details();

		int loop = 0;

		if(position > 0)
		{
			FlashReadData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) position * BARCODE_DETAILS_LEN)),barcode_details[position].arr,BARCODE_DETAILS_LEN);
		}
		else
		{
			if(mc.mc.prod_totalcount > 0)
			{
				for(loop = 0 ; (loop <= mc.mc.prod_totalcount) && (loop <= MAX_PRODUCTS) ; loop++)
				{
					FlashReadData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) loop * BARCODE_DETAILS_LEN)),barcode_details[loop].arr,BARCODE_DETAILS_LEN);
				}
			}
			else
			{
				for(loop = 0 ; loop <= MAX_PRODUCTS ; loop++)
				{
					FlashReadData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) loop * BARCODE_DETAILS_LEN)),barcode_details[loop].arr,BARCODE_DETAILS_LEN);
				}
			}
		}
	}
}
//-------------------------------------------------------------------------
void store_barcode_details(int position)
{
//	if(usb_barcode_enable == TRUE)
//	{
		int loop = 0;
		if(position > 0)
		{
			strcpy(barcode_details[position].barcodeDetails.barcode,product_bacode);
			barcode_details[position].barcodeDetails.checksum = product_bacode_checksum;
			FlashModifyData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) position * BARCODE_DETAILS_LEN)),barcode_details[position].arr,BARCODE_DETAILS_LEN);
		}
		else
		{
			for(loop = 0 ; loop <= mc.mc.prod_totalcount ; loop++)
			{
				FlashModifyData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) loop * BARCODE_DETAILS_LEN)),barcode_details[loop].arr,BARCODE_DETAILS_LEN);
			}
		}
//	}
}
//-------------------------------------------------------------------------
void delete_barcode_details(int position)
{
	if(usb_barcode_enable == TRUE)
	{
		int loop = 0;
		if(position > 0)
		{
			memset(barcode_details[position].barcodeDetails.barcode,0,sizeof(barcode_details[position].barcodeDetails.barcode));
			barcode_details[position].barcodeDetails.checksum = 0;
			FlashModifyData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) position * BARCODE_DETAILS_LEN)),barcode_details[position].arr,BARCODE_DETAILS_LEN);
		}
		else
		{
			memset(barcode_details[position].barcodeDetails.barcode,0,sizeof(barcode_details[position].barcodeDetails.barcode));
			barcode_details[position].barcodeDetails.checksum = 0;
			for(loop = 0 ; loop <= mc.mc.prod_totalcount ; loop++)
			{
				FlashModifyData((((UINT32) BARCODE_DETAILS_PG * PGBYTS) + ((UINT32) loop * BARCODE_DETAILS_LEN)),barcode_details[loop].arr,BARCODE_DETAILS_LEN);
			}
		}
	}
}

//-------------------------------------------------------------------------
int search_for_duplicate_barcode_details( int exceptThisPosition )
{
	int loop = 0;
	char tempBuff[20]={0};

	clrscr();
	double_height = 1;
	gotoxy(3,1);
	lcdprint("  SEARCHING PRODUCT ");
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT....  ");

	for(loop = 0 ; loop <= mc.mc.prod_totalcount ; loop++)
	{
		if(loop == exceptThisPosition)
			continue;
		if(barcode_details[loop].barcodeDetails.checksum > 0)
		{
			if(barcode_details[loop].barcodeDetails.checksum == product_bacode_checksum)
			{
				if(0 == strcmp(product_bacode,barcode_details[loop].barcodeDetails.barcode))
				{
					return loop;
				}
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
int search_for_product_using_barcode_details(void)		//Mrugnil 30/01/24
{
	int loop = 0;
	char tempBuff[20]={0};

	clrscr();
	double_height = 1;
	gotoxy(3,1);
	lcdprint("  SEARCHING PRODUCT ");
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT....  ");

	for(loop = 0 ; loop <= mc.mc.prod_totalcount ; loop++)
	{
		if(barcode_details[loop].barcodeDetails.checksum > 0)
		{
			if(barcode_details[loop].barcodeDetails.checksum == product_bacode_checksum)
			{
				if(0 == strcmp(product_bacode,barcode_details[loop].barcodeDetails.barcode))
				{
					return loop;
				}
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
int scan_barcode()
{
	char key = 0, loop = 0;

	clrscr();
	double_height = 1;
	gotoxy(3,1);
	lcdprint("  SCANNING BARCODE  ");
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT...   ");
	double_height = 0;
	gotoxy(8,1);
	lcdprint("                 ESC");

	target_init();
	product_bacode_checksum = 0;
	memset(product_bacode, 0, sizeof(product_bacode));

	while(1)
	{
		if(TRUE == usb_barcode_available)//nilesh
		{
			strcpy(product_bacode,barcode_arr);
			memset(barcode_arr,0,sizeof(barcode_arr));
			barcode_count = 0;
			usb_barcode_available = FALSE;
			for(loop = 0 ; loop < strlen(product_bacode) ; loop++)
				product_bacode_checksum += product_bacode[loop];
			return TRUE;
		}
		key = keyPressCheck();
		if(key == '*')
			break;
	}
	clrscr();
	double_height = 1;
	gotoxy(3,1);
	lcdprint("BARCODE SCAN FAILED!");
	Delay_ms(2000);
	return FALSE;
}

int read_product_barcode()
{
	product_bacode_checksum = 0;

	if(TRUE == scan_barcode())
	{

	}
	return product_bacode_checksum;
}
//-------------------------------------------------------------------------
void clear_flash_for_barcode_details()
{
	char i = 0;

	clrscr();
	double_height = 1;
	gotoxy(2,1);
	lcdprint("CLEARING BARCODE PGS");
	gotoxy(4,1);
	lcdprint("PLEASE WAIT....");
	memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	for(i = 0; i < 9; i++) //clearing barcode details
	{
		FlashModifyData(((unsigned long)(BARCODE_DETAILS_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 4096);
	}
	FlashModifyData(((unsigned long)(BARCODE_DETAILS_PG * PGBYTS) + ((unsigned long)(i * 4096))), g_8K_membuf, 3136);
}
//-------------------------------------------------------------------------
void get_mass_storage_barcode_usb_keyboard_settings( void )
{
	usb_barcode_enable = FALSE;
	usb_keyboard_enable = FALSE;
	if(mc.mc.massStrg_barcd_USBKbd_sett == MASS_STORAGE_SETTING)
	{
		usb_barcode_enable = FALSE;
		usb_keyboard_enable = FALSE;
	}
	if(mc.mc.massStrg_barcd_USBKbd_sett == BARCODE_SETTING)
	{
		usb_barcode_enable = TRUE;
		usb_keyboard_enable = FALSE;
	}
	if(mc.mc.massStrg_barcd_USBKbd_sett == USB_KEYBOARD_SETTING)
	{
		usb_barcode_enable = FALSE;
		usb_keyboard_enable = TRUE;
	}
}
//--------------------------------------------------------------------------
void sendDataGPRS(unsigned char allTypeFlag)
{
//	UINT32 lastAdd = 0, tempdata = 0, startAddress = 0, startLocation = 0;
//	volatile UINT32 mainloop = 0;
//	UINT16 countGData = 0, i = 0, totalRecordsToSend = 0, recordsSent = 0, totalPackets = 0, sentPackets = 0;
//	UCHAR entry = 0, sentRecord = 0, opPos = 0, packet_fail = 0;
//
//	if(modem_connectivity == FALSE)
//	{
//		clrscr();
//		double_height = 1;
//		gotoxy(3,1);
//		lcdprint(" COMMUNICATION ERROR");
//		beep(ERRRBEEP);
//		Delay_ms(2000);
//		mc.mc.gprs_interval_data = 0;
//		store_mc();
//		return;
//	}
//
//	mc.mc.checkIntervalFail = 1;
//	store_mc();
//
//	if(mc.mc.currentPG)
//		lastAdd = mc.mc.recs;
//	else
//		lastAdd = mc.mc.rec1;
//
//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint("  COUNTING RECORDS  ");
//	double_height = 0;
//	gotoxy(5,1);
//	lcdprint(" RECORDS: ");
//
//	if(allTypeFlag)
//		startLocation = mc.mc.GPRS_rec_start;
//	else
//		startLocation = 1;
//
//	for(mainloop = startLocation; mainloop < lastAdd; mainloop++)
//	{
//		hex_asc(mainloop, asc, 5);
//		rem_zeros(asc, 4);
//		gotoxy(5,12);
//		disp_char(asc[0]);
//		disp_char(asc[1]);
//		disp_char(asc[2]);
//		disp_char(asc[3]);
//		disp_char(asc[4]);
//
//		get_rec(mainloop);
//		if((!rec.rec.gprs_send_rec_flag) && (rec.rec.bill_stat != 'O'))
//			totalRecordsToSend++;
//	}
//	totalPackets = (totalRecordsToSend / SEND_REC_GPRS);
//	if((totalRecordsToSend % SEND_REC_GPRS) >= 1)
//		totalPackets += 1;
//
//	clrscr();
//	double_height = 1;
//	double_width = 0;
//	gotoxy(3,1);
//	lcdprint(" TOTAL RECS TO SEND ");
//	double_height = 0;
//	gotoxy(5,1);
//	lcdprint(" RECORDS: ");
//	hex_asc(totalRecordsToSend, asc, 5);
//	rem_zeros(asc, 4);
//	for(i = 0; i < 5; i++)
//		disp_char(asc[i]);
//	gotoxy(6,1);
//	lcdprint(" PACKETS: ");
//	hex_asc(totalPackets, asc, 5);
//	rem_zeros(asc, 4);
//	for(i = 0; i < 5; i++)
//		disp_char(asc[i]);
//
//	if(!totalRecordsToSend)
//	{
//		clrscr();
//		double_height = 1;
//		double_width = 0;
//		gotoxy(3,1);
//		lcdprint("  NO RECORDS FOUND  ");
//		beep(ERRRBEEP);
//		Delay_ms(2000);
//		mc.mc.gprs_interval_data = 0;
//		mc.mc.checkIntervalFail = 0;
//		store_mc();
//		return;
//	}
//
//	if(FALSE == connect_gprs())
//	{
//		mc.mc.checkIntervalFail = 0;
//		store_mc();
//		return;
//	}
//
//	countGData = 0;
//	memset(g_8K_membuf, 0, (sizeof(g_8K_membuf) - 5));
//	strcpy(g_8K_membuf,"data=");
//	countGData = strlen(g_8K_membuf);
//
//	FlashReadSecID();
//
//	sentRecord = 0;
//	gprsFlag_ptr = 0;
//
//	if(mc.mc.currentPG)
//		lastAdd = mc.mc.recs;
//	else
//		lastAdd = mc.mc.rec1;
//
//	for(mainloop = startLocation, sentPackets = 0; mainloop < lastAdd; mainloop++)
//	{
//		get_rec(mainloop);
//		if((!rec.rec.gprs_send_rec_flag) && (rec.rec.bill_stat != 'O'))
//			entry = 1;
//		else
//			entry = 0;
//
//		if(entry)
//		{
//			if(!startAddress)
//				startAddress = mainloop;
//
//			if(rec.rec.last != 1)
//			{
//				sentRecord++;
//				recordsSent++;
//
//				for(i = MACHINE_ID_START; i < (MACHINE_ID_START + ETIMNO_LEN); i++)
//					g_8K_membuf[countGData++] = (SecID_Buffer[i]);
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = ('C');
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = (rec.rec.strt_byt);
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = ('S');
//				g_8K_membuf[countGData++] = (',');
//				//RECEIPT NUMBER
//				hex_asc(rec.rec.rcpt_no, asc, 5);
//				for(i = 0; i < 5; i++)
//					g_8K_membuf[countGData++] = (asc[i]);
//				g_8K_membuf[countGData++] = (',');
//
//				//DATE & TIME
//				dd2[1] = (rec.rec.dd  & 0x0f) | 0x30;
//				dd2[0] = ((rec.rec.dd & 0xf0) >> 4) | 0x30;
//				mm2[1] = (rec.rec.mm  & 0x0f) | 0x30;
//				mm2[0] = ((rec.rec.mm & 0xf0) >> 4) | 0x30;
//				yy2[1] = (rec.rec.yy  & 0x0f) | 0x30;
//				yy2[0] = ((rec.rec.yy & 0xf0) >> 4) | 0x30;
//				hr2[1] = (rec.rec.hr  & 0x0f) | 0x30;
//				hr2[0] = ((rec.rec.hr & 0xf0) >> 4) | 0x30;
//				mn2[1] = (rec.rec.mn  & 0x0f) | 0x30;
//				mn2[0] = ((rec.rec.mn & 0xf0) >> 4) | 0x30;
//				sc2[1] = (rec.rec.sc  & 0x0f) | 0x30;
//				sc2[0] = ((rec.rec.sc & 0xf0) >> 4) | 0x30;
//
//				g_8K_membuf[countGData++] = (dd2[0]);
//				g_8K_membuf[countGData++] = (dd2[1]);
//				g_8K_membuf[countGData++] = ('/');
//				g_8K_membuf[countGData++] = (mm2[0]);
//				g_8K_membuf[countGData++] = (mm2[1]);
//				g_8K_membuf[countGData++] = ('/');
//				g_8K_membuf[countGData++] = (yy2[0]);
//				g_8K_membuf[countGData++] = (yy2[1]);
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = (hr2[0]);
//				g_8K_membuf[countGData++] = (hr2[1]);
//				g_8K_membuf[countGData++] = (':');
//				g_8K_membuf[countGData++] = (mn2[0]);
//				g_8K_membuf[countGData++] = (mn2[1]);
//				g_8K_membuf[countGData++] = (':');
//				g_8K_membuf[countGData++] = (sc2[0]);
//				g_8K_membuf[countGData++] = (sc2[1]);
//				g_8K_membuf[countGData++] = (',');
//
//				if(rec.rec.product_code == 'D')
//				{
//					g_8K_membuf[countGData++] = ('0');
//					g_8K_membuf[countGData++] = ('0');
//					g_8K_membuf[countGData++] = ('0');
//					g_8K_membuf[countGData++] = ('0');
//					g_8K_membuf[countGData++] = (',');
//				}
//				else if((rec.rec.product_code != 'D') && (rec.rec.product_code > 0))
//				{
//					//PRODCUT CODE
//					FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_SER_NO), asc, PRODUCT_SER_NO_LEN);
//					for(i = 0; i < PRODUCT_SER_NO_LEN; i++)
//						g_8K_membuf[countGData++] = (asc[i]);
//					g_8K_membuf[countGData++] = (',');
//				}
//				else
//				{
//					for(i = 0; i < PRODUCT_SER_NO_LEN; i++)
//						g_8K_membuf[countGData++] = (' ');
//					g_8K_membuf[countGData++] = (',');
//				}
//
//				if(rec.rec.product_code == 'D')
//				{
//					g_8K_membuf[countGData++] = ('D');
//					g_8K_membuf[countGData++] = ('E');
//					g_8K_membuf[countGData++] = ('L');
//					g_8K_membuf[countGData++] = ('E');
//					g_8K_membuf[countGData++] = ('T');
//					g_8K_membuf[countGData++] = ('E');
//					g_8K_membuf[countGData++] = ('D');
//					g_8K_membuf[countGData++] = (' ');
//					g_8K_membuf[countGData++] = ('I');
//					g_8K_membuf[countGData++] = ('T');
//					g_8K_membuf[countGData++] = ('E');
//					g_8K_membuf[countGData++] = ('M');
//					g_8K_membuf[countGData++] = (',');
//				}
//				else if((rec.rec.product_code != 'D') && (rec.rec.product_code > 0))
//				{
//					//PRODUCT NAME
//					FlashReadData((((UINT32) PRODUCT_PG * PGBYTS) + ((UINT32) rec.rec.product_code * PRODUCT_LEN) + PRODUCT_TEXT), asc, PRODUCT_TEXT_LEN);
//					for(i = 0; i < PRODUCT_TEXT_LEN; i++)
//					{
//						if((asc[i] == '&') || (asc[i] == '(') || (asc[i] == ')') || (asc[i] == '@') || (asc[i] == '*') || (asc[i] == '/') || (asc[i] == '-') || (asc[i] == '+'))
//							g_8K_membuf[countGData++] = (' ');
//						else
//							g_8K_membuf[countGData++] = (asc[i]);
//					}
//					g_8K_membuf[countGData++] = (',');
//				}
//				else
//				{
//					for(i = 0; i < PRODUCT_TEXT_LEN; i++)
//						g_8K_membuf[countGData++] = (' ');
//					g_8K_membuf[countGData++] = (',');
//				}
//
//				//PRODUCT RATE
//				hex_asc(rec.rec.rate, asc, 8);
//				for(i = 0; i < 8; i++)
//				{
//					if(i == 6)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//FRACTONAL DIGIT
//				if(rec.rec.unit_fract)
//				{
//					hex_asc(rec.rec.quantity, asc, 6);
//					for(i = 0; i < 6; i++)
//					{
//						if(i == 3)
//							g_8K_membuf[countGData++] = ('.');
//						g_8K_membuf[countGData++] = (asc[i]);
//					}
//				}
//				else
//				{
//					tempdata = (rec.rec.quantity / 1000);
//					hex_asc(tempdata, asc, 6);
//					g_8K_membuf[countGData++] = ('0');
//					for(i = 0; i < 6; i++)
//						g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				if(rec.rec.unit_type)
//				{
//					//UNIT SYMBOL
//					FlashReadData((((UINT32) UNIT_TYPE_PG * PGBYTS) + ((UINT32) rec.rec.unit_type * UNIT_TYPE_LEN) + UNIT_TYPE_SYMBOL), asc, UNIT_TYPE_SYMBOL_LEN);
//					for(i = 0; i < UNIT_TYPE_SYMBOL_LEN; i++)
//						g_8K_membuf[countGData++] = (asc[i]);
//					g_8K_membuf[countGData++] = (',');
//				}
//				else
//				{
//					for(i = 0; i < UNIT_TYPE_SYMBOL_LEN; i++)
//						g_8K_membuf[countGData++] = (' ');
//					g_8K_membuf[countGData++] = (',');
//				}
//
//				//PRODUCT AMOUNT
//				value_hex_asc_64U = rec.rec.final_prod_amount;
//				hex_asc_64U(asc, 12);
//				for(i = 0; i < 12; i++)
//				{
//					if(i == 10)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//PRODUCT SGST
//				hex_asc(rec.rec.sgst_amt, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//PRODUCT SGST
//				hex_asc(rec.rec.prd_sgst, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//PRODUCT SGST
//				hex_asc(rec.rec.cgst_amt, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//PRODUCT SGST
//				hex_asc(rec.rec.prd_cgst, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//PRODUCT DISCOUNT
//				hex_asc(rec.rec.discount_amt, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//VOID FLAG
//				g_8K_membuf[countGData++] = rec.rec.bill_stat;
//				g_8K_membuf[countGData++] = (',');
//				if(rec.rec.calc_stat)
//					g_8K_membuf[countGData++] = rec.rec.calc_stat;
//				else
//					g_8K_membuf[countGData++] = '0';
//			}
//			else
//			{
//				sentRecord++;
//				recordsSent++;
//
//				for(i = MACHINE_ID_START; i < (MACHINE_ID_START + ETIMNO_LEN); i++)
//					g_8K_membuf[countGData++] = (SecID_Buffer[i]);
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = ('C');
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = (rec.rec.strt_byt);
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = ('L');
//				g_8K_membuf[countGData++] = (',');
//
//				//RECEIPT NUMBER
//				hex_asc(rec.rec.rcpt_no, asc, 5);
//				for(i = 0; i < 5; i++)
//					g_8K_membuf[countGData++] = (asc[i]);
//				g_8K_membuf[countGData++] = (',');
//
//				//DATE & TIME
//				dd2[1] = (rec.rec.dd  & 0x0f) | 0x30;
//				dd2[0] = ((rec.rec.dd & 0xf0) >> 4) | 0x30;
//				mm2[1] = (rec.rec.mm  & 0x0f) | 0x30;
//				mm2[0] = ((rec.rec.mm & 0xf0) >> 4) | 0x30;
//				yy2[1] = (rec.rec.yy  & 0x0f) | 0x30;
//				yy2[0] = ((rec.rec.yy & 0xf0) >> 4) | 0x30;
//				hr2[1] = (rec.rec.hr  & 0x0f) | 0x30;
//				hr2[0] = ((rec.rec.hr & 0xf0) >> 4) | 0x30;
//				mn2[1] = (rec.rec.mn  & 0x0f) | 0x30;
//				mn2[0] = ((rec.rec.mn & 0xf0) >> 4) | 0x30;
//				sc2[1] = (rec.rec.sc  & 0x0f) | 0x30;
//				sc2[0] = ((rec.rec.sc & 0xf0) >> 4) | 0x30;
//
//				g_8K_membuf[countGData++] = (dd2[0]);
//				g_8K_membuf[countGData++] = (dd2[1]);
//				g_8K_membuf[countGData++] = ('/');
//				g_8K_membuf[countGData++] = (mm2[0]);
//				g_8K_membuf[countGData++] = (mm2[1]);
//				g_8K_membuf[countGData++] = ('/');
//				g_8K_membuf[countGData++] = (yy2[0]);
//				g_8K_membuf[countGData++] = (yy2[1]);
//				g_8K_membuf[countGData++] = (',');
//				g_8K_membuf[countGData++] = (hr2[0]);
//				g_8K_membuf[countGData++] = (hr2[1]);
//				g_8K_membuf[countGData++] = (':');
//				g_8K_membuf[countGData++] = (mn2[0]);
//				g_8K_membuf[countGData++] = (mn2[1]);
//				g_8K_membuf[countGData++] = (':');
//				g_8K_membuf[countGData++] = (sc2[0]);
//				g_8K_membuf[countGData++] = (sc2[1]);
//				g_8K_membuf[countGData++] = (',');
//
//				//FINAL AMOUNT
//				value_hex_asc_64U = rec.rec.final_prod_amount;
//				hex_asc_64U(asc, 12);
//				for(i = 0; i < 12; i++)
//				{
//					if(i == 10)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//FINAL SGST
//				hex_asc(rec.rec.sgst_amt, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//FINAL DISCOUNT
//				hex_asc(rec.rec.discount_amt, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//CGST
//				hex_asc(rec.rec.cgst_amt, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//CGST
//				hex_asc(rec.rec.other_chrg, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//CGST
//				hex_asc(rec.rec.other_chrg_rate, asc, 9);
//				for(i = 0; i < 9; i++)
//				{
//					if(i == 7)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//TOTAL BILL AMOUNT
//				value_hex_asc_64U = rec.rec.total_amt;
//				hex_asc_64U(asc, 12);
//				for(i = 0; i < 12; i++)
//				{
//					if(i == 10)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//FINAL BILL AMOUNT
//				value_hex_asc_64U = rec.rec.round_off_tot_amt;
//				hex_asc_64U(asc, 12);
//				for(i = 0; i < 12; i++)
//				{
//					if(i == 10)
//						g_8K_membuf[countGData++] = ('.');
//					g_8K_membuf[countGData++] = (asc[i]);
//				}
//				g_8K_membuf[countGData++] = (',');
//
//				//VOID FLAG
//				g_8K_membuf[countGData++] = rec.rec.bill_stat;
//				g_8K_membuf[countGData++] = ',';
//				if(rec.rec.calc_stat)
//					g_8K_membuf[countGData++] = rec.rec.calc_stat;
//				else
//					g_8K_membuf[countGData++] = '0';
//				g_8K_membuf[countGData++] = (',');
//
//				opPos = searchAgent(rec.rec.agent_no);
//				if(opPos)
//				{
//					FlashReadData((((unsigned long)AGENT_PG * PGBYTS) + ((unsigned long)opPos * AGENT_TOTAL_LEN)), asc, AGENT_TOTAL_LEN);
//					for(i = AGENT_SHORT_NAME; i < (AGENT_SHORT_NAME + AGENT_SHORT_NAME_LEN); i++)
//						g_8K_membuf[countGData++] = (asc[i]);
//				}
//				else
//				{
//					for(i = 0; i < (AGENT_SHORT_NAME_LEN); i++)
//						g_8K_membuf[countGData++] = (' ');
//				}
//			}
//
//			if(sentRecord > (SEND_REC_GPRS - 1))
//			{
//				sentPackets++;
//				clrscr();
//				double_height = 1;
//				double_width = 0;
//				gotoxy(3,1);
//				lcdprint("   SENDING PACKET   ");
//				double_height = 0;
//				hex_asc(sentPackets, asc, 5);
//				rem_zeros(asc,4);
//				gotoxy(5,3);
//				for(i = 0; i < 5; i++)
//				{
//					disp_char(asc[i]);
//				}
//				disp_char(' ');
//				disp_char('O');
//				disp_char('F');
//				disp_char(' ');
//				hex_asc(totalPackets, asc, 5);
//				rem_zeros(asc,4);
//				for(i = 0; i < 5; i++)
//				{
//					if(asc[i] != ' ')
//						disp_char(asc[i]);
//				}
//				sentRecord = 0;
////				if(send_data_to_server(g_8K_membuf))
//				if(send_data_to_server("H"))
//					store_gprs_flag(startAddress, mainloop);
//				startAddress = 0;
//				gprsFlag_ptr = 0;
//				countGData = 0;
//				memset(g_8K_membuf, 0, (sizeof(g_8K_membuf) - 5));
//				strcpy(g_8K_membuf,"data=");
//				countGData = strlen(g_8K_membuf);
//			}
//			else if((sentRecord > 0) && (recordsSent >= totalRecordsToSend))
//			{
//				sentPackets++;
//				clrscr();
//				double_height = 1;
//				double_width = 0;
//				gotoxy(3,1);
//				lcdprint("   SENDING PACKET   ");
//				double_height = 0;
//				hex_asc(sentPackets, asc, 5);
//				rem_zeros(asc,4);
//				gotoxy(5,3);
//				for(i = 0; i < 5; i++)
//				{
//					disp_char(asc[i]);
//				}
//				disp_char(' ');
//				disp_char('O');
//				disp_char('F');
//				disp_char(' ');
//				hex_asc(totalPackets, asc, 5);
//				rem_zeros(asc,4);
//				for(i = 0; i < 5; i++)
//				{
//					if(asc[i] != ' ')
//						disp_char(asc[i]);
//				}
//				sentRecord = 0;
////				if(send_data_to_server(g_8K_membuf))
//				if(send_data_to_server("H"))
//					store_gprs_flag(startAddress, mainloop);
//				else
//					packet_fail = 1;
//				startAddress = 0;
//				gprsFlag_ptr = 0;
//				countGData = 0;
//				memset(g_8K_membuf, 0, (sizeof(g_8K_membuf) - 5));
//				strcpy(g_8K_membuf,"data=");
//				countGData = strlen(g_8K_membuf);
//			}
//			else
//			{
//				g_8K_membuf[countGData++] = ('|');
//			}
//		}
//	}
//
//	if(!packet_fail)
//		mc.mc.GPRS_rec_start = lastAdd;
//
//	if(mc.mc.gprs_en_dis == RECORD_LOT)
//	{
//		mc.mc.gprs_record_sent = 0;
//	}
//	else if(mc.mc.gprs_en_dis == TIME_INTERVAL_LOT)
//	{
//		read_rtc();
//		tim_hex_asc();
//		i = asc_hex(hr2, 2);
////		if((i + mc.mc.gprs_recordsLot) > 23)
////		{
////			mc.mc.gprs_record_sent = ((i + mc.mc.gprs_recordsLot) - 24);
////		}
////		else
////		{
////			mc.mc.gprs_record_sent = (i + mc.mc.gprs_recordsLot);
////		}
//		mc.mc.gprs_record_sent = i;
//	}
//
//	store_mc();
//	disconnect_gprs();
//	clrscr();
//	mc.mc.gprs_interval_data = 0;
//	mc.mc.checkIntervalFail = 0;
//	store_mc();
}
//-------------------------------------------------------------------------
void store_gprs_flag(unsigned long start, unsigned long end)
{
	UINT32 loop = 0, i = 0, count = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(3,1);
	lcdprint("  UPDATING RECORDS  ");
	double_height = 0;
	gotoxy(5,1);
	lcdprint(" RECORDS: ");
    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	for(loop = start, count = 0; loop <= end; loop++)
	{
		hex_asc(loop, asc, 5);
		rem_zeros(asc, 4);
		gotoxy(5,12);
		disp_char(asc[0]);
		disp_char(asc[1]);
		disp_char(asc[2]);
		disp_char(asc[3]);
		disp_char(asc[4]);

		get_rec(loop);

		if((!rec.rec.gprs_send_rec_flag) && (rec.rec.bill_stat != 'O'))
		{
			rec.rec.gprs_send_rec_flag = 1;
			for(i = 0; i < RECDAT_LEN; i++)
				g_8K_membuf[count++] = rec.arr[i];
		}
	}

	if(count)
	{
		if(mc.mc.currentPG)
			FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) + ((unsigned long)(start * RECDAT_LEN))), g_8K_membuf, count);
		else
			FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) + ((unsigned long)(start * RECDAT_LEN))), g_8K_membuf, count);
	}

	gprsFlag_ptr = 0;
}
//-------------------------------------------------------------------------
void sendDayWiseSummary_SMSToAllMobileNumbers(void)
{
	get_mobile_details();
	if(mob_details.mob_details.mobile1_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB1");
		Delay_ms(1000);
		strncpy(mobileNumber, mob_details.mob_details.mobile1, 10);
		sendSMSDayWiseSummary();
	}
	if(mob_details.mob_details.mobile2_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB2");
		Delay_ms(1000);
		strncpy(mobileNumber, mob_details.mob_details.mobile2, 10);
		sendSMSDayWiseSummary();
	}
	if(mob_details.mob_details.mobile3_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB3");
		Delay_ms(1000);
		strncpy(mobileNumber, mob_details.mob_details.mobile3, 10);
		sendSMSDayWiseSummary();
	}
}
//-------------------------------------------------------------------------
void sendSMSDayWiseSummary(void)
{
	char buff[200], i = 0, j = 0;

	mc.mc.checkIntervalFail = 1;
	store_mc();

	get_gsm_gprs();
//	if(modem_connectivity == FALSE)
//	{
//		clrscr();
//		double_height = 1;
//		gotoxy(3,1);
//		lcdprint(" COMMUNICATION ERROR");
//		beep(ERRRBEEP);
//		Delay_ms(2000);
//		mc.mc.checkIntervalFail = 0;
//		store_mc();
//		clrscr();
//		return;
//	}

	if(daily.daily.daily_set_var == 'Y')
	{
		FlashReadData((((UINT32) HEADER_PG * PGBYTS) + ((UINT32) 1 * HDR_LEN)), membuf, HDR_LEN);
		memset(buff, 0, 200);
		for(i = HDR_NM, j = 0; i < (HDR_NM + HDR_NM_LEN); i++)
		{
			if((membuf[i] != ' '))
			{
				buff[j++] = membuf[i];
			}
		}
		strcat(buff, "; TOTAL BILLS:");
		hex_asc(daily.daily.dailyBillNo, asc, 5);
		rem_zeros(asc, 4);
		strcat(buff, asc);

		strcat(buff, "; TOTAL AMOUNT: ");
		value_hex_asc_64U = daily.daily.dailyTotAmnt;
		hex_asc_64U(membuf, 12);
		rem_zeros(membuf, 8);
		for(i = 0, j = 0; i < 12; i++)
		{
			if(i == 10)
				asc[j++] = '.';

			if(membuf[i] >= '0' && membuf[i] <= '9')
				asc[j++] = membuf[i];
		}
		asc[j] = 0;
		strcat(buff, asc);

		if(daily.daily.dailyTotDisc)
		{
			strcat(buff, "; TOTAL DISC:");
			value_hex_asc_64U = daily.daily.dailyTotDisc;
			hex_asc_64U(membuf, 12);
			rem_zeros(membuf, 8);
			for(i = 0, j = 0; i < 12; i++)
			{
				if(i == 10)
					asc[j++] = '.';

				if(membuf[i] >= '0' && membuf[i] <= '9')
					asc[j++] = membuf[i];
			}
			asc[j] = 0;
			strcat(buff, asc);
		}
		if(daily.daily.dailyTotTax)
		{
			strcat(buff, "; TOTAL TAX:");
			value_hex_asc_64U = daily.daily.dailyTotTax;
			hex_asc_64U(membuf, 12);
			rem_zeros(membuf, 8);
			for(i = 0, j = 0; i < 12; i++)
			{
				if(i == 10)
					asc[j++] = '.';

				if(membuf[i] >= '0' && membuf[i] <= '9')
					asc[j++] = membuf[i];
			}
			asc[j] = 0;
			strcat(buff, asc);
		}

		if(sendCustomSMS(buff,mobileNumber))
		{
			if(mc.mc.gsm_en_dis == RECORD_LOT)
			{
				mc.mc.gsm_record_sent = 0;
				store_mc();
			}
			else if(mc.mc.gsm_en_dis == TIME_INTERVAL_LOT)
			{
				read_rtc();
				tim_hex_asc();
				j = asc_hex(hr2, 2);
//				if((j + mc.mc.gsm_recordsLot) > 23)
//				{
//					mc.mc.gsm_record_sent = ((j + mc.mc.gsm_recordsLot) - 24);
//				}
//				else
//				{
//					mc.mc.gsm_record_sent = (j + mc.mc.gsm_recordsLot);
//				}
				mc.mc.gsm_record_sent = j;
				mc.mc.gsm_interval_data = 0;
				store_mc();
			}
			mc.mc.checkIntervalFail = 0;
			store_mc();
		}
		clrscr();
	}
	clrscr();
}
//-------------------------------------------------------------------------
unsigned char searchAgent(unsigned int opcode)
{
	UCHAR loopCount = 0;

	for(loopCount = 0; loopCount <= mc.mc.operator_totalcount; loopCount++)
	{
		FlashReadData((((unsigned long)AGENT_PG * PGBYTS) + ((unsigned long)loopCount * AGENT_TOTAL_LEN) + AGENT_CODE), asc, AGENT_CODE_LEN);

		if(opcode == asc_hex(asc, AGENT_CODE_LEN))
			return loopCount;
	}
	return 0;
}
//-------------------------------------------------------------------------
void checkHourandSendData(void)
{
	UCHAR entry = 0, hr = 0, mn = 0;
	UINT32 recs = 0;
	UINT32 sentHourInMins = 0, currentHourInMins = 0, intervalHourInMins = 0;

//	if(modem_connectivity == TRUE)
//	{
		if(mc.mc.currentPG)
			recs = mc.mc.recs;
		else
			recs = mc.mc.rec1;

		if((mc.mc.udf_en_dis) && (!udf_no))
			entry = 1;
		else if((!mc.mc.udf_en_dis) && (!recArrayPtr))
			entry = 1;
		else
			entry = 0;

		if(recs > 2)
			entry = 1;

		if(mc.mc.checkIntervalFail)
			entry = 0;

		if(entry)
		{
			if(mc.mc.gsm_en_dis == TIME_INTERVAL_LOT)
			{
				entry = 0;
				read_rtc();
				tim_hex_asc();
				hr = asc_hex(hr2, 2);
				mn = asc_hex(mn2, 2);

				sentHourInMins = 0;
				intervalHourInMins = 0;
				currentHourInMins = 0;

				sentHourInMins = mc.mc.gsm_record_sent * 60;
				intervalHourInMins = mc.mc.gsm_recordsLot * 60;
				currentHourInMins = hr * 60;
				currentHourInMins += mn;
				if(mc.mc.gsm_interval_data)
				{
					if(sentHourInMins > currentHourInMins)
					{
						if(mc.mc.gsm_record_sent > 0)
							sentHourInMins = (24 - mc.mc.gsm_record_sent) * 60;
						else
							sentHourInMins = 0;
					}
					if((currentHourInMins - sentHourInMins) > intervalHourInMins)
					{
						get_mobile_details();
						if(mob_details.mob_details.mobile1_set == 'Y')
						{
							clrscr();
							double_height = 1;
							gotoxy(3,1);
							lcdprint(" SENDING SMS TO MOB1");
							Delay_ms(1000);
							strncpy(mobileNumber, mob_details.mob_details.mobile1, 10);
							sendSMSDayWiseSummary();
						}
						if(mob_details.mob_details.mobile2_set == 'Y')
						{
							clrscr();
							double_height = 1;
							gotoxy(3,1);
							lcdprint(" SENDING SMS TO MOB2");
							Delay_ms(1000);
							strncpy(mobileNumber, mob_details.mob_details.mobile2, 10);
							sendSMSDayWiseSummary();
						}
						if(mob_details.mob_details.mobile3_set == 'Y')
						{
							clrscr();
							double_height = 1;
							gotoxy(3,1);
							lcdprint(" SENDING SMS TO MOB3");
							Delay_ms(1000);
							strncpy(mobileNumber, mob_details.mob_details.mobile3, 10);
							sendSMSDayWiseSummary();
						}
						clrscr();
					}
				}
			}

//			if(mc.mc.gprs_en_dis == TIME_INTERVAL_LOT)
//			{
//				entry = 0;
//				read_rtc();
//				tim_hex_asc();
//				hr = asc_hex(hr2, 2);
//				mn = asc_hex(mn2, 2);
//
//				sentHourInMins = 0;
//				intervalHourInMins = 0;
//				currentHourInMins = 0;
//
//				sentHourInMins = mc.mc.gprs_record_sent * 60;
//				intervalHourInMins = mc.mc.gprs_recordsLot * 60;
//				currentHourInMins = hr * 60;
//				currentHourInMins += mn;
//				if(mc.mc.gprs_interval_data)
//				{
//					if(sentHourInMins > currentHourInMins)
//					{
//						if(mc.mc.gprs_record_sent > 0)
//							sentHourInMins = (24 - mc.mc.gprs_record_sent) * 60;
//						else
//							sentHourInMins = 0;
//					}
//					if((currentHourInMins - sentHourInMins) > intervalHourInMins)
//					{
//						pkt4SavePacketToModemUnsentDataBulk(mc.mc.lastTicket);
//					}
//				}
//			}
		}
//	}
}
//-------------------------------------------------------------------------
void get_product_names()
{
	short int loopCount = 0;

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(3,1);
	lcdprint("  CLEARING PRODUCTS ");
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT...   ");

	for(loopCount = 0 ; loopCount <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH ; loopCount++)
	{
		productNameArr[loopCount].productNameArr.en_dis_flag = 0;
		productNameArr[loopCount].productNameArr.chArr[0] = 0;
		productNameArr[loopCount].productNameArr.chArr[1] = 0;
	}

	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(3,1);
	lcdprint("  GETTING PRODUCTS  ");
	gotoxy(5,1);
	lcdprint("   PLEASE WAIT...   ");
	for(loopCount = 1 ; (loopCount <= mc.mc.prod_totalcount) && (loopCount <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH) ; loopCount++)
	{
		FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) loopCount * PRODUCT_LEN)), asc, PRODUCT_LEN);
		productNameArr[loopCount].productNameArr.en_dis_flag = asc[PRODUCT_EN_FLAG];
		productNameArr[loopCount].productNameArr.chArr[0] = asc[PRODUCT_TEXT];
		productNameArr[loopCount].productNameArr.chArr[1] = asc[PRODUCT_TEXT+1];
	}
}
//-------------------------------------------------------------------------
//unsigned short int get_matching_product(char * enteredStr, unsigned char enteredDigCount)
//{
//	char foundFlag = 1, searchOnFlashFlag = 0, compareDigCount = 0;
//	int loop = 0, i = 0, count = 0;
//
//	for(i = 0 ; i < sizeof(g_ID_buf) ; i++)
//	{
//		g_ID_buf[i] = 0;
//	}
//	if(enteredDigCount == 0)
//	{
//		for(loop = 1 ; (loop <= mc.mc.prod_totalcount) && (loop <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH) ; loop++)
//		{
//			if(productNameArr[loop].productNameArr.en_dis_flag == ',')
//				g_ID_buf[count++] = loop;
//		}
//		return count;
//	}
//	compareDigCount = enteredDigCount;
//	if(enteredDigCount > 2)
//	{
//		searchOnFlashFlag = 1;
//		compareDigCount = 2;
//	}
//	for(loop = 1 ; (loop <= mc.mc.prod_totalcount) && (loop <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH) ; loop++)
//	{
//		if(productNameArr[loop].productNameArr.en_dis_flag == ',')
//		{
//			foundFlag = 1;
//			for(i = 0 ; i < compareDigCount ; i++)
//			{
//				if(productNameArr[loop].productNameArr.chArr[i] != enteredStr[i])
//				{
//					foundFlag = 0;
//					break;
//				}
//			}
//			if(foundFlag)
//			{
//				if(searchOnFlashFlag)
//				{
//					FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) loop * PRODUCT_LEN)) + PRODUCT_TEXT, asc, PRODUCT_TEXT_LEN);
//					foundFlag = 1;
//					for(i = 0 ; i < enteredDigCount ; i++)
//					{
//						if(enteredStr[i] != asc[i])
//						{
//							foundFlag = 0;
//							break;
//						}
//					}
//					if(foundFlag)
//					{
//						g_ID_buf[count++] = loop;
//					}
//				}
//				else
//				{
//					g_ID_buf[count++] = loop;
//				}
//			}
//		}
//	}
//	return count;
//}
//-------------------------------------------------------------------------
unsigned short int get_matching_product(char * enteredStr, unsigned char enteredDigCount)
{
	char foundFlag = 1, searchOnFlashFlag = 0, compareDigCount = 0, searchByInitials = 0;
	int loop = 0, i = 0, count = 0;
	char *tempArry;

	for(i = 0 ; i < sizeof(g_ID_buf) ; i++)
	{
		g_ID_buf[i] = 0;
	}
	if(enteredDigCount == 0)
	{
		for(loop = 1 ; (loop <= mc.mc.prod_totalcount) && (loop <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH) ; loop++)
		{
			if(productNameArr[loop].productNameArr.en_dis_flag == ',')
				g_ID_buf[count++] = loop;
		}
		return count;
	}
	compareDigCount = enteredDigCount;
	if(*(enteredStr + 1) == ' ')
	{
		searchByInitials = 1;
		searchOnFlashFlag = 0;
		compareDigCount = 1;
	}
	else if(enteredDigCount > 2)
	{
		searchByInitials = 0;
		searchOnFlashFlag = 1;
		compareDigCount = 2;
	}
	memset(asc, 0, 100);
	for(loop = 1 ; (loop <= mc.mc.prod_totalcount) && (loop <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH) ; loop++)
	{
		if(productNameArr[loop].productNameArr.en_dis_flag == ',')
		{
			if(searchByInitials)
			{
				foundFlag = 1;
				for(i = 0 ; i < compareDigCount ; i++)
				{
					if(productNameArr[loop].productNameArr.chArr[i] != enteredStr[i])
					{
						foundFlag = 0;
						break;
					}
				}
				if(foundFlag)
				{
					tempArry = &enteredStr[1];
					FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) loop * PRODUCT_LEN)) + PRODUCT_TEXT, asc, PRODUCT_TEXT_LEN);
					if(strstr(asc, tempArry))
					{
						g_ID_buf[count++] = loop;
					}
				}
			}
			else
			{
				foundFlag = 1;
				for(i = 0 ; i < compareDigCount ; i++)
				{
					if(productNameArr[loop].productNameArr.chArr[i] != enteredStr[i])
					{
						foundFlag = 0;
						break;
					}
				}
				if(foundFlag)
				{
					if(searchOnFlashFlag)
					{
						FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) loop * PRODUCT_LEN)) + PRODUCT_TEXT, asc, PRODUCT_TEXT_LEN);
						foundFlag = 1;
						for(i = 0 ; i < enteredDigCount ; i++)
						{
							if(enteredStr[i] != asc[i])
							{
								foundFlag = 0;
								break;
							}
						}
						if(foundFlag)
						{
							g_ID_buf[count++] = loop;
						}
					}
					else
					{
						g_ID_buf[count++] = loop;
					}
				}
			}
		}
	}
	return count;
}
//-------------------------------------------------------------------------
int search_prd_name()
{
	unsigned char exit = 0, key = 0, prdNameStrChanged = 0;
	unsigned char upScroll = 0, downScroll = 0;
	unsigned short int prdCount = 0;
	short int displayFromPos = 0;
	char prdNameArr[21] = {0};
	signed int digCount = 0;
	short int loopCount = 0,storeCount = 0, selectedPrdPos = 0;
	int printFlag = 0;

	shiftFlag = 0;
	move_frwd_flag = 1;
	doubleKeyPressCheck = 0;
	prdCount = get_matching_product("",0);//to get all the products for the first time

	memset(prdNameArr,0,sizeof(prdNameArr));
	double_width = 0;
	clrscr();
	double_height = 1;
	gotoxy(1,1);
	lcdprint("PRODUCT NAME : ");

	display_product_names(displayFromPos,prdCount);
	while(!exit)
	{
		if(prdNameStrChanged)
		{
			double_height = 0;
			gotoxy(3,1);
			lcdprint(prdNameArr);
//			prdCount = get_matching_product(prdNameArr,digCount);
			prdCount = get_matching_product(prdNameArr,digCount);
		}
		if(upScroll || downScroll || prdNameStrChanged)
		{
			display_product_names(displayFromPos,prdCount);
		}
		upScroll = 0;
		downScroll = 0;
		prdNameStrChanged = 0;

		while(!(key = keyPressCheck()));
		switch(key)
		{
		case PRINT_KEY:
			if(mc.mc.product_selection == BEFORE_QTY)
			{
				if(!recArrayPtr && !returnBill && searchProductSingle)
				{
					if(prdCount > 0)
					{
						selectedPrdPos = g_ID_buf[displayFromPos];
						exit = 1;
						printFlag = 1;
					}
				}
			}
			break;
		case MENU_KEY:
//			if(!recArrayPtr && !returnBill)
//			{
//				if(prdCount > 0)
//				{
//					selectedPrdPos = g_ID_buf[displayFromPos];
//					exit = 1;
//					printFlag = 2;
//				}
//			}
			if(!mc.mc.udf_en_dis && !returnBill)
			{
				if(prdCount > 0)
				{
					selectedPrdPos = g_ID_buf[displayFromPos];
					exit = 1;
					printFlag = 2;
				}
			}
			break;
		case '*':
			exit = 1;
			selectedPrdPos = 0;
			break;
		case '#':
			if(prdCount > 0)
				selectedPrdPos = g_ID_buf[displayFromPos];
			else
				selectedPrdPos = 0;
			exit = 1;
			break;
		case SHIFT_KEY:
			shiftFlag = ~shiftFlag;
			if(shiftFlag)
			{
				double_height = 0;
				double_width = 0;
				gotoxy(8,1);
				lcdprint("SHIFT");
			}
			else
			{
				double_height = 0;
				double_width = 0;
				gotoxy(8,1);
				lcdprint("     ");
			}
			break;
		case UP_ARROW:
			if(shiftFlag)
			{
				--digCount;
				if(digCount > 0)
					prdNameArr[digCount] = ' ';
				else
				{
					digCount = 0;
					prdNameArr[digCount] = ' ';
					displayFromPos = 0;
				}
				prdNameStrChanged = 1;
			}
			else
			{
				upScroll = 1;
				displayFromPos--;
				if(displayFromPos < 0)
					displayFromPos = 0;
			}
			break;
		case DOWN_ARROW:
			downScroll = 1;
			displayFromPos++;
			if(displayFromPos >= prdCount-1)
				displayFromPos = prdCount-1;
			break;
		default:
			if((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9'))
			{
				if(digCount < 20)
				{
					if((doubleKeyPressCheckWait > 0) && (!move_frwd_flag))
					{
						prdNameArr[digCount - 1] = key;
						doubleKeyPressCheckWait = 0;
					}
					else if((shiftFlag) && (key == '0'))
					{
						prdNameArr[digCount++] = ' ';
						doubleKeyPressCheckWait = 0;
					}
					else
						prdNameArr[digCount++] = key;
					prdNameStrChanged = 1;
				}
			}
			break;
		}
	}

	if((prdCount > 0) && (selectedPrdPos > 0))
	{
		product_code = selectedPrdPos;
		FlashReadData(((unsigned long)PRODUCT_PG*PGBYTS) + ((unsigned long)product_code * PRODUCT_LEN), membuf, PRODUCT_LEN);
		switch(slab_no)
		{
			case 1:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT_LEN - 1);
					break;
			case 2:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT2_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT2 + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT2 + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT2_LEN - 1);
					break;
			case 0:
			case 3:
					for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_AMOUNT3_LEN; loopCount++)
					{
						if(membuf[PRODUCT_AMOUNT3 + loopCount] != '.')
						{
							asc[storeCount++] = membuf[PRODUCT_AMOUNT3 + loopCount];
						}
					}
					final_bill_amount = asc_hex(asc, PRODUCT_AMOUNT3_LEN - 1);
					break;
		}

		//get product discount
		for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_DISCOUNT_LEN; loopCount++)
		{
			if(membuf[PRODUCT_DISCOUNT + loopCount] != '.')
			{
				asc[storeCount++] = membuf[PRODUCT_DISCOUNT + loopCount];
			}
		}
		prod_discount = asc_hex(asc, PRODUCT_DISCOUNT_LEN - 1);

		//get product SGST
		for(loopCount = 0,storeCount = 0; loopCount < PRODUCT_SGST_LEN; loopCount++)
		{
			if(membuf[PRODUCT_SGST + loopCount] != '.')
			{
				asc[storeCount++] = membuf[PRODUCT_SGST + loopCount];
			}
		}
		prod_vat = asc_hex(asc, PRODUCT_SGST_LEN - 1);

		unit_type = asc_hex(&membuf[PRODUCT_UNIT_TYPE], PRODUCT_UNIT_TYPE_LEN);

		FlashReadData(((unsigned long)UNIT_TYPE_PG*PGBYTS) + ((unsigned long)unit_type * UNIT_TYPE_LEN), asc, UNIT_TYPE_LEN);
		unit_fract = asc_hex(&asc[UNIT_TYPE_FRAC_QTY], UNIT_TYPE_FRAC_QTY_LEN);

		if(printFlag == 1)
		{
			oneProductSale(product_code);
			return 2;
		}
		else if(printFlag == 2)
		{
			return 3;
		}
		else
			return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------
void display_product_names(short int fromPos, short int prodCount)
{
	unsigned short int loop = 0;
	unsigned char doOnce = 0, rowPos = 4, displayedPrdCount = 0;

	double_height = 1;
	gotoxy(4,1);
	lcdprint("                    ");
	gotoxy(6,1);
	lcdprint("                    ");
	double_height = 0;
	double_width = 0;
	for(loop = fromPos,rowPos = 4 ; (loop < prodCount) && (displayedPrdCount < 3) ; loop++,displayedPrdCount++)
	{
		memset(asc, ' ', PRODUCT_TEXT_LEN);
		double_height = 0;
		gotoxy(rowPos,1);
		FlashReadData(((( UINT32) PRODUCT_PG * PGBYTS) + (( UINT32) g_ID_buf[loop] * PRODUCT_LEN)) + PRODUCT_TEXT, asc, (PRODUCT_TEXT_LEN-7));
		if(!doOnce)
		{
			doOnce = 1;
			double_height = 1;
		}
		lcdprint(asc);
		if(double_height)
		{
			++rowPos;
			++rowPos;
		}
		else
			++rowPos;
	}
}
//-------------------------------------------------------------------------
//void delete_prd_from_name_wise_search(unsigned int pos, unsigned char deleteAllPrdFlag)
//{
//	unsigned short int loop = 0;
//	if(mc.mc.prod_totalcount <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH)
//	{
//		if(deleteAllPrdFlag)
//		{
//			for(loop = 0 ; loop <= mc.mc.prod_totalcount ; loop++)
//			{
//				productNameArr[loop].productNameArr.en_dis_flag = 1;
//				productNameArr[loop].productNameArr.chArr[0] = ' ';
//				productNameArr[loop].productNameArr.chArr[1] = ' ';
//			}
//		}
//		if(pos <= MAX_PRODUCTS_FOR_FAST_NAMEWISE_SEARCH)
//		{
//			productNameArr[pos].productNameArr.en_dis_flag = 1;
//			productNameArr[pos].productNameArr.chArr[0] = ' ';
//			productNameArr[pos].productNameArr.chArr[1] = ' ';
//		}
//	}
//}
//-------------------------------------------------------------------------
void load256ProductSummaryinTempArray(unsigned int start)
{
	UINT16 loop = 0;
	UCHAR i = 0;

	for(loop = start; (loop <= MAX_PRODUCTS) && (loop <= mc.mc.prod_totalcount); loop++)
	{
		get_product_summary(loop, mc.mc.currentPG);

		for(i = 0; i < PRODUCT_SUMMARY_LEN; i++)
			temp_product_summary[(loop - start) + 1].arr[i] = product_summary.arr[i];
	}

	loop = 0;
}
//-------------------------------------------------------------------------
unsigned int loadRecordsinTempArray(unsigned long start)
{
	UINT32 loop = 0;
	UCHAR i = 0;
	UINT32 recs = 0;

	if(mc.mc.currentPG)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	for(loop = start; (loop < (start + TEMP_RECS_DISC_ARRAY)) && (loop < recs); loop++)
	{
		get_rec(loop);

		for(i = 0; i < RECDAT_LEN; i++)
			save_rec[loop - start].arr[i] = rec.arr[i];
		flagIGST = rec.rec.igstFlag;
	}

	return loop;
}
//-------------------------------------------------------------------------
void loadProductsStockTempArray(unsigned int start)
{
	UINT16 loop = 0;

	for(loop = start; (loop <= MAX_PRODUCTS) && (loop <= mc.mc.prod_totalcount); loop++)
	{
		get_product_stock(loop);
		temp_product_stock[(loop - start) + 1].temp_product_stock.totalStock = product_stock.product_stock.totalStock;
		temp_product_stock[(loop - start) + 1].temp_product_stock.sign = product_stock.product_stock.sign;
	}

	loop = 0;
}
//-------------------------------------------------------------------------
void loadTaxSummaryinTempArray(unsigned int start, unsigned int taxCount, unsigned char taxType, unsigned char pageSelect)
{
	UINT16 loop = 0;
	UCHAR i = 0;
	for(loop = start; (loop < (start + TEMP_TAX_SUMM_ARRAY)) && (loop < taxCount); loop++)
	{
		if(taxType == SGST)
		{
			get_sgst_summ(loop, pageSelect);
			for(i = 0; i < TAX_SUMMARY_LEN; i++)
				SGST_tax_summary[(loop - start)].arr[i] = sgst_summary.arr[i];
		}
		else if(taxType == CGST)
		{
			get_cgst_summ(loop, pageSelect);
			for(i = 0; i < TAX_SUMMARY_LEN; i++)
				CGST_tax_summary[(loop - start)].arr[i] = cgst_summary.arr[i];
		}
		else if(taxType == IGST)
		{
			get_igst_summ(loop, pageSelect);
			for(i = 0; i < TAX_SUMMARY_LEN; i++)
				IGST_tax_summary[(loop - start)].arr[i] = igst_summary.arr[i];
		}
	}
	loop = 0;
}
//-------------------------------------------------------------------------
void voidBillInBulk(unsigned long receiptNo, unsigned char pageSelect)
{
	UINT32 loopCount = 0, recs = 0, RECORD_START_ADD = 0, loopProdSumm = 0, reccount = 0, loop = 0, tax_amount = 0;
	UINT16 totalRecs = 0;
	UCHAR i = 0, found = 0, billType = 0;
	UINT32 tempVat = 0, tempServiceTax = 0, start = 0, end = 0;
	if(pageSelect)
		recs = mc.mc.recs;
	else
		recs = mc.mc.rec1;

	sgstArrayPtr = 0;
	cgstArrayPtr = 0;
	recArrayPtr = 0;
	g_prodPosArrayPtr = 0;
	g_prodArrayPtr = 0;
	tempVat = 0;
	tempServiceTax = 0;
	start = 0;
	end = 0;

	for(loopCount = 1; loopCount < recs; loopCount++)
	{
		get_rec_report(loopCount, pageSelect);

		if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.last != 1) && (rec.rec.calc_stat != 'C'))
		{
			for(i = 0; i < RECDAT_LEN; i++)
				recArray[recArrayPtr][i] = rec.arr[i];
			recArrayPtr++;
			sgstArray[sgstArrayPtr++] = rec.rec.prd_sgst;
			cgstArray[cgstArrayPtr++] = rec.rec.prd_cgst;
			tempVat += rec.rec.sgst_amt;
			tempServiceTax += rec.rec.cgst_amt;
			g_ID_buf[g_prodPosArrayPtr++] = rec.rec.product_code;
			prd_array[g_prodArrayPtr].prd_array.rate = rec.rec.rate;
			prd_array[g_prodArrayPtr++].prd_array.product_code = rec.rec.product_code;

			billType = rec.rec.calc_stat;
			if(!start)
				start = loopCount;
		}
		else if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.last == 1) && (rec.rec.calc_stat != 'C'))
		{
			end = loopCount;
			break;
		}
	}

	sortVATAndServiceArray();

	/*******************************************************************
	 * START >> Change Status of Bill
	 *******************************************************************/

	totalRecs = (recs / TEMP_RECS_DISC_ARRAY);
	if((recs % TEMP_RECS_DISC_ARRAY) > 0)
		totalRecs += 1;

	RECORD_START_ADD = 1;
	for(loopProdSumm = 0; loopProdSumm < totalRecs; loopProdSumm++)
	{
		reccount = loadRecordsinTempArray(RECORD_START_ADD);

		if(reccount > recs)
			break;

		found = 0;
		for(loop = RECORD_START_ADD; (loop < recs) && (loop < ((RECORD_START_ADD + TEMP_RECS_DISC_ARRAY))); loop++)
		{
			for(i = 0; i < RECDAT_LEN; i++)
				rec.arr[i] = save_rec[loop - RECORD_START_ADD].arr[i];

			if((rec.rec.rcpt_no == receiptNo) && (rec.rec.bill_stat == 'C') && (rec.rec.calc_stat != 'C'))
			{
				rec.rec.bill_stat = 'V';
				rec.rec.gprs_send_rec_flag = 0;
				found = 1;
			}

			for(i = 0; i < RECDAT_LEN; i++)
				save_rec[loop - RECORD_START_ADD].arr[i] = rec.arr[i];
		}
		if(found)
		{
			for(loop = 0, loopCount = 0; (loop < TEMP_RECS_DISC_ARRAY); loop++)
			{
				for(i = 0; i < RECDAT_LEN; i++)
					g_8K_membuf[loopCount++] = save_rec[loop].arr[i];
			}

			if(pageSelect)
				FlashModifyData((((unsigned long)(REC_PG * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
			else
				FlashModifyData((((unsigned long)(REC_PG_1 * PGBYTS)) +
						((unsigned long)(RECORD_START_ADD * RECDAT_LEN))), g_8K_membuf, loopCount);
		}

	    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));

		RECORD_START_ADD += TEMP_RECS_DISC_ARRAY;
	}

	/*******************************************************************
	 * END >> Change Status of Bill
	 *******************************************************************/

	if(billType != 'C')
	{

		/*******************************************************************
		 * START >> Update TAX15DAY Summary
		 *******************************************************************/
		if(billType == 0)
			manageTAX366DaySummaryAndStore(0);
		else
			manageTAX366DaySummaryAndStore(1);
		/*******************************************************************
		 * END >> Update TAX15DAY Summary
		 *******************************************************************/

		/*******************************************************************
		 * START >> Update Stock Summary
		 *******************************************************************/
		if(mc.mc.stock_manage_en_dis)
		{
			if(billType == 0)
				manageStockFronRAMAndStore(0);
			else if(billType == 'R')
				manageStockFronRAMAndStore(1);
		}
		/*******************************************************************
		 * END >> Update Stock Summary
		 *******************************************************************/


		/*******************************************************************
		 * START >> Update Product Summary
		 *******************************************************************/
		if(billType == 0)
			manageProductSummaryFromRAMAndStore(0);
		else if(billType == 'R')
			manageProductSummaryFromRAMAndStore(1);
		/*******************************************************************
		 * END >> Update Product Summary
		 *******************************************************************/


		/*******************************************************************
		 * START >> Update Tax Summary
		 *******************************************************************/
		if(pageSelect == mc.mc.currentPG)
		{
			if(billType == 0)
				manageTAXFromRAMAndStore(SGST, 0);
			else if(billType == 'R')
				manageTAXFromRAMAndStore(SGST, 1);
		}
		else
		{
			for(loopCount = 0; loopCount < sgstArrayPtr; loopCount++)
			{
				for(loop = 0, tax_amount = 0; loop < recArrayPtr; loop++)
				{
					for(i = 0; i < RECDAT_LEN; i++)
						temprec.arr[i] = recArray[loop][i];

					if(temprec.temprec.prd_sgst == sgstArray[loopCount])
					{
						if(temprec.temprec.prd_sgst)
							tax_amount += temprec.temprec.sgst_amt;
						else
							tax_amount += temprec.temprec.final_prod_amount;
					}
				}

				if(tax_amount)
				{
					if(billType == 0)
						checkUpdateGST(sgstArray[loopCount], tax_amount, 0, 0, 0, 0, 0);
					else if(billType == 'R')
						checkUpdateGST(sgstArray[loopCount], tax_amount, 0, 0, 0, 0, 1);
				}
			}
		}

		if(pageSelect == mc.mc.currentPG)
		{
			if(billType == 0)
				manageTAXFromRAMAndStore(CGST, 0);
			else if(billType == 'R')
				manageTAXFromRAMAndStore(CGST, 1);
		}
		else
		{
			for(loopCount = 0; loopCount < cgstArrayPtr; loopCount++)
			{
				for(loop = 0, tax_amount = 0; loop < recArrayPtr; loop++)
				{
					for(i = 0; i < RECDAT_LEN; i++)
						temprec.arr[i] = recArray[loop][i];

					if(temprec.temprec.prd_cgst == cgstArray[loopCount])
					{
						if(temprec.temprec.prd_cgst)
							tax_amount += temprec.temprec.cgst_amt;
						else
							tax_amount += temprec.temprec.final_prod_amount;
					}
				}

				if(tax_amount)
				{
					if(billType == 0)
						checkUpdateGST(0, 0, cgstArray[loopCount], tax_amount, 0, 0, 0);
					else if(billType == 'R')
						checkUpdateGST(0, 0, cgstArray[loopCount], tax_amount, 0, 0, 1);
				}
			}
		}
		/*******************************************************************
		 * END >> Update Tax Summary
		 *******************************************************************/

		/*******************************************************************
		 * START >> Update Summary
		 *******************************************************************/
		get_rec_report(end, pageSelect);
		if(rec.rec.calc_stat == 'R')
		{
			if(voidTotalAmount > rec.rec.round_off_tot_amt)
				voidTotalAmount -= rec.rec.round_off_tot_amt;
			else
				voidTotalAmount = 0;
	//		get_daily_summary(mc.mc.current_dd);
			if(daily.daily.dailyBillNo > 0)
				daily.daily.dailyBillNo--;
			daily.daily.dailyTotAmnt += rec.rec.round_off_tot_amt;
			daily.daily.dailyTotDisc += rec.rec.discount_amt;
			daily.daily.dailyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
			store_daily_summary(mc.mc.current_dd);

	//		get_monthly_summary(mc.mc.current_mm);
			if(monthly.monthly.monthlyBillNo > 0)
				monthly.monthly.monthlyBillNo--;
			monthly.monthly.monthlyTotAmnt += rec.rec.round_off_tot_amt;
			monthly.monthly.monthlyTotDisc += rec.rec.discount_amt;
			monthly.monthly.monthlyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
			store_monthly_summary(mc.mc.current_mm);

	//		get_yearly_summary(mc.mc.current_yy);
			if(yearly.yearly.yearlyBillNo > 0)
				yearly.yearly.yearlyBillNo--;
			yearly.yearly.yearlyTotAmnt += rec.rec.round_off_tot_amt;
			yearly.yearly.yearlyTotDisc += rec.rec.discount_amt;
			yearly.yearly.yearlyTotTax += (rec.rec.cgst_amt + rec.rec.sgst_amt);
			store_yearly_summary(mc.mc.current_yy);
		}
		else
		{
			voidTotalAmount += rec.rec.round_off_tot_amt;
	//		get_daily_summary(mc.mc.current_dd);
			if(daily.daily.dailyBillNo > 0)
				daily.daily.dailyBillNo--;
			if(daily.daily.dailyTotAmnt >= rec.rec.round_off_tot_amt)
				daily.daily.dailyTotAmnt -= rec.rec.round_off_tot_amt;
			else
				daily.daily.dailyTotAmnt = 0;
			if(daily.daily.dailyTotDisc >= rec.rec.discount_amt)
				daily.daily.dailyTotDisc -= rec.rec.discount_amt;
			else
				daily.daily.dailyTotDisc = 0;
			if(daily.daily.dailyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
				daily.daily.dailyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
			else
				daily.daily.dailyTotTax = 0;
			store_daily_summary(mc.mc.current_dd);

	//		get_monthly_summary(mc.mc.current_mm);
			if(monthly.monthly.monthlyBillNo > 0)
				monthly.monthly.monthlyBillNo--;
			if(monthly.monthly.monthlyTotAmnt >= rec.rec.round_off_tot_amt)
				monthly.monthly.monthlyTotAmnt -= rec.rec.round_off_tot_amt;
			else
				monthly.monthly.monthlyTotAmnt = 0;
			if(monthly.monthly.monthlyTotDisc >= rec.rec.discount_amt)
				monthly.monthly.monthlyTotDisc -= rec.rec.discount_amt;
			else
				monthly.monthly.monthlyTotDisc = 0;
			if(monthly.monthly.monthlyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
				monthly.monthly.monthlyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
			else
				monthly.monthly.monthlyTotTax = 0;
			store_monthly_summary(mc.mc.current_mm);

	//		get_yearly_summary(mc.mc.current_yy);
			if(yearly.yearly.yearlyBillNo > 0)
				yearly.yearly.yearlyBillNo--;
			if(yearly.yearly.yearlyTotAmnt >= rec.rec.round_off_tot_amt)
				yearly.yearly.yearlyTotAmnt -= rec.rec.round_off_tot_amt;
			else
				yearly.yearly.yearlyTotAmnt = 0;
			if(yearly.yearly.yearlyTotDisc >= rec.rec.discount_amt)
				yearly.yearly.yearlyTotDisc -= rec.rec.discount_amt;
			else
				yearly.yearly.yearlyTotDisc = 0;
			if(yearly.yearly.yearlyTotTax >= (rec.rec.cgst_amt + rec.rec.sgst_amt))
				yearly.yearly.yearlyTotTax -= (rec.rec.cgst_amt + rec.rec.sgst_amt);
			else
				yearly.yearly.yearlyTotTax = 0;
			store_yearly_summary(mc.mc.current_yy);
		}

		/*******************************************************************
		 * END >> Update Summary
		 *******************************************************************/
	}
	sgstArrayPtr = 0;
	cgstArrayPtr = 0;
	recArrayPtr = 0;
	g_prodPosArrayPtr = 0;
}
//-------------------------------------------------------------------------
void voidBWRReportBills(unsigned long fromRcpt, unsigned long toRcpt, char pageFlag)
{
	UINT16 loopCount = 0, recForDay = 0, dataCount = 0, bwr_ptr = 0;
	UCHAR i = 0;
	UINT32 recCountLoop = 0, start = 0;

	if(pageFlag == 2)
	{
		bwr_ptr = rpt_details.rpt_details.bwr_ptr;
	}
	else
	{
		if(pageFlag)
		{
			get_rec_report((mc.mc.recs - 1), pageFlag);
		}
		else
		{
			get_rec_report((mc.mc.rec1 - 1), pageFlag);
		}

		for(loopCount = 1; loopCount <= 366; loopCount++)
		{
			get_bwr_rpt(0, loopCount);

			pk_unpk(bwr.bwr.dd, dd2);
			pk_unpk(bwr.bwr.mm, mm2);
			pk_unpk(bwr.bwr.yy, yy2);

			if((rec.rec.dd == bwr.bwr.dd) && (rec.rec.mm == bwr.bwr.mm) && (rec.rec.yy == bwr.bwr.yy))
			{
				bwr_ptr = loopCount;
				break;
			}
		}
	}

//	switch(bwr_ptr)
//	{
//		case 1:
//				recForDay = rpt_details.rpt_details.bwr1_count;
//				break;
//		case 2:
//				recForDay = rpt_details.rpt_details.bwr2_count;
//				break;
//		case 3:
//				recForDay = rpt_details.rpt_details.bwr3_count;
//				break;
//		case 4:
//				recForDay = rpt_details.rpt_details.bwr4_count;
//				break;
//		case 5:
//				recForDay = rpt_details.rpt_details.bwr5_count;
//				break;
//		case 6:
//				recForDay = rpt_details.rpt_details.bwr6_count;
//				break;
//		case 7:
//				recForDay = rpt_details.rpt_details.bwr7_count;
//				break;
//		case 8:
//				recForDay = rpt_details.rpt_details.bwr8_count;
//				break;
//		case 9:
//				recForDay = rpt_details.rpt_details.bwr9_count;
//				break;
//		case 10:
//				recForDay = rpt_details.rpt_details.bwr10_count;
//				break;
//		case 11:
//				recForDay = rpt_details.rpt_details.bwr11_count;
//				break;
//		case 12:
//				recForDay = rpt_details.rpt_details.bwr12_count;
//				break;
//		case 13:
//				recForDay = rpt_details.rpt_details.bwr13_count;
//				break;
//		case 14:
//				recForDay = rpt_details.rpt_details.bwr14_count;
//				break;
//		case 15:
//				recForDay = rpt_details.rpt_details.bwr15_count;
//				break;
//	}
	get_rpt_details();
	get_rpt_details_count(rpt_details.rpt_details.bwr_ptr);

	recForDay = rpt_details_count.rpt_details_count.bwr_count;
	for(loopCount = 0, recCountLoop = 0, dataCount = 0; loopCount < recForDay; loopCount++)
	{
		get_bwr_rpt(loopCount, bwr_ptr);

		if((bwr.bwr.rcpt_no >= fromRcpt) && (bwr.bwr.rcpt_no <= toRcpt))
		{
			bwr.bwr.bill_stat = 'V';
		}

		for(i = 0; i < BWR_UNION_LEN; i++)
			g_8K_membuf[dataCount++] = bwr.arr[i];

		recCountLoop++;
		if(recCountLoop > TEMP_BWR_RECS)
		{
			store_bwr_bulk(start, g_8K_membuf, dataCount, bwr_ptr);
			start += recCountLoop;
			recCountLoop = 0;
			dataCount = 0;
			if(bwr.bwr.rcpt_no >= toRcpt)
				break;
		}
		else if((recCountLoop <= TEMP_BWR_RECS) && (loopCount == (recForDay - 1)))
		{
			store_bwr_bulk(start, g_8K_membuf, dataCount, bwr_ptr);
			start += recCountLoop;
			recCountLoop = 0;
			dataCount = 0;
			if(bwr.bwr.rcpt_no >= toRcpt)
				break;
		}
	}
}
//-------------------------------------------------------------------------
unsigned long long floatExtractionU64(unsigned long long amount, unsigned long rate, unsigned char calcType)
{
    unsigned long long MAmount = 0, MAmountRateProduct = 0, MAmountRateProductRF5 = 0, MGSTRate = 0, finalAmt = 0;
    unsigned long long var = 0;

    UCHAR taxChecking[PRODUCT_AMOUNT_LEN + 1] = {0}, printingLoop = 0;		//Mrugnil 02/07/24

    memset(taxChecking, 0, PRODUCT_AMOUNT_LEN);		//Mrugnil 02/07/24

	if(mc.mc.qty_enter_method != QTY_3_3)
		rate = (rate * 10);

	MAmount = (amount * 10);

    MAmountRateProduct = (MAmount * rate);

    if(calcType == FINAL_PRODUCT_AMOUNT)
    {
        /* Depending on Quantity Entry divide rate here*/ //vikas optional but not required in this logic
        MAmountRateProduct = (MAmountRateProduct / 1000);
        MAmountRateProductRF5 = (MAmountRateProduct + 5);
        finalAmt = (MAmountRateProductRF5 / 10);
    }
    else if((calcType == TAX) || (calcType == DISCOUNT))
    {
    	if(mc.mc.qty_enter_method != QTY_3_3)
            MAmountRateProduct = (MAmountRateProduct / 10000);
    	else
    		MAmountRateProduct = (MAmountRateProduct / 1000);
        MAmountRateProductRF5 = (MAmountRateProduct + 5);
        finalAmt = (MAmountRateProductRF5 / 100);
    }
    else if(calcType == MRP && flagIGST == 1 )
	{
    	if(mc.mc.qty_enter_method != QTY_3_3)
    		finalAmt =  (amount*rate)/(100000+(rate));
    	else
    		finalAmt =  (amount*rate)/(10000+(rate));
	}
    else if(calcType == MRP && flagIGST == 0)
    {
        MGSTRate = (rate * 2);
        if(mc.mc.qty_enter_method != QTY_3_3)
        	var = (MAmountRateProduct/(100000 + MGSTRate));
        else
        	var = (MAmountRateProduct/(10000 + MGSTRate));
//        finalAmt = (var / 10);
    	if(mc.mc.reverse_tax_en_dis)
    		finalAmt = (var);
    	else
    		finalAmt = (var/10);
    }
    else if(calcType == TAX_RPT)
    {
    	if(mc.mc.qty_enter_method != QTY_3_3)
    		var = (MAmount * 100000);
    	else
    		var = (MAmount * 10000);
        var = (var / rate);
    	if(mc.mc.reverse_tax_en_dis)
    		finalAmt = (var/100);
    	else
    		finalAmt = (var/10);
//        finalAmt = (var / 10);
    }
    return finalAmt;
}
//-------------------------------------------------------------------------
unsigned long long floatCalculatePercentageU64(unsigned long long amount, unsigned long rate)
{
	float famount = 0.0, frate = 0.0, var = 0.0;
	float Fpart = 0;
	UINT64 Ipart = 0, finalAmt = 0, tempVal = 0;
	unsigned char buff[50], dotPosition = 0;

	famount = (float) amount / 100;
	frate = (float) rate / 100;

	var = (frate / famount);
	var = var * 100.0;

    Ipart = (unsigned long) var;
    Fpart = var - Ipart;

    memset(buff, '\0', 50);

    value_hex_asc_64U = Ipart;
    hex_asc_64U(buff, 10);

    dotPosition = strlen(buff);

    Fpart = Fpart * 100.0;
    Fpart += 0.5;

    memset(asc, '\0', 10);
    tempVal = (unsigned long long)Fpart;

    value_hex_asc_64U = tempVal;
    hex_asc_64U(asc, 2);

    buff[dotPosition] = asc[0];
    buff[dotPosition + 1] = asc[1];

    finalAmt = asc_hex64U(buff, 12);

    return finalAmt;
}
//-------------------------------------------------------------------------
void pc_comm_mode(void)
{
	//	put usb otg in device mode
	psp_usb_otg_set_id(PSP_USB_OTG_ID_DEVICE);

	clrscr();

	while(TRUE)
	{
		while(keyPressCheck() != '*')
		{
			USB_CDC_Scheduler();

			if(ConnectedToPC == FALSE)
			{
				double_height = 1;
				gotoxy(1, 1);
				lcdprint("  PC COMMUNICATION  ");
				double_height = 0;
				gotoxy(7, 1);
				lcdprint("  'CANCEL' TO EXIT  ");
				double_height = 1;
				gotoxy(4, 1);
				lcdprint("  WAITING FOR PC..  ");
				double_height = 0;
			}
			else
			{
				double_height = 1;
				gotoxy(1, 1);
				lcdprint("  PC COMMUNICATION  ");
				double_height = 0;
				gotoxy(7, 1);
				lcdprint("  'CANCEL' TO EXIT  ");
				double_height = 1;
				gotoxy(4, 1);
				lcdprint(" CONNECTED TO PC... ");
				double_height = 0;
			}

			chk_comm();

		}
		break;
	}
	//	put usb otg in host mode
	psp_usb_otg_set_id(PSP_USB_OTG_ID_HOST);
}
//-------------------------------------------------------------------------
unsigned char date_entry(unsigned char * msg, unsigned char type)
{
	UCHAR exit = 0;
	UCHAR i = 0, key = 0, count = 0;
	UCHAR dd = 0, mm = 0, yy = 0;
	unsigned char date[12] = {0}, start = 0;

	memset(date, '\0', sizeof(date));

	if(type == DATE)
	{
		date[0] = dd2[0];
		date[1] = dd2[1];

		date[2] = '/';

		date[3] = mm2[0];
		date[4] = mm2[1];

		date[5] = '/';

		date[6] = yy2[0];
		date[7] = yy2[1];
	}
	else if(type == MONTH_YEAR)
	{
		date[0] = ' ';
		date[1] = ' ';

		date[2] = ' ';

		date[3] = mm2[0];
		date[4] = mm2[1];

		date[5] = '/';

		date[6] = yy2[0];
		date[7] = yy2[1];
	}
	else if(type == HOUR_MIN)
	{
		date[0] = ' ';
		date[1] = ' ';

		date[2] = ' ';

		date[3] = hr2[0];
		date[4] = hr2[1];

		date[5] = '/';

		date[6] = mn2[0];
		date[7] = mn2[1];
	}

	if(type == DATE)
		start = 0;
	else if((type == MONTH_YEAR) || (type == HOUR_MIN))
		start = 3;

//	count = 0;
	count = start;
	clrscr();
	while(!exit)
	{
		double_height = 0;
		double_height = 1;
		gotoxy(2, 1);
		lcdprint(msg);
		gotoxy(4, 6);
		for(i = 0 ; i < 8 ; i++)
			disp_char(date[i]);

		double_height = 0;
		gotoxy(8, 1);
		lcdprint("             ESC/ENT");
		double_height = 1;

		while(!(key = keyPressCheck()))
		{
			if(count < 8)
			{
				if((count == 2) || (count == 5))
					gotoxy(4, (6 + count + 1));
				else
					gotoxy(4, (6 + count));
			}
			else
				gotoxy(4, (6 + 7));

			if(blink_flag)
			{
				disp_char('_');
			}
			else
			{
				if(count < 8)
				{
					if((count == 2) || (count == 5))
						disp_char(date[count + 1]);
					else
						disp_char(date[count]);
				}
				else
					disp_char(date[7]);
			}
		}
		switch(key)
		{
			case UP_ARROW:
						if(count < 7)
						{
							if((count == 3) || (count == 6))
								count += 2;
							else
								count++;
						}
						break;
			case DOWN_ARROW:
//						if(count)
						if(count > start)
						{
							if((count == 3) || (count == 6))
								count -= 2;
							else
								count--;
						}
						break;
			case '*':
						return 0;
			case '#':
						asc[0] = date[0];
						asc[1] = date[1];
						dd = asc_hex(asc, 2);
						asc[0] = date[3];
						asc[1] = date[4];
						mm = asc_hex(asc, 2);
						asc[0] = date[6];
						asc[1] = date[7];
						yy = asc_hex(asc, 2);
						if(type == DATE)
						{
							if(check_for_valid_date(dd, mm, yy))
							{
								temp_dd = dd;
								temp_mm = mm;
								temp_yy = yy;
								return 1;
							}
							else
							{
								clrscr();
								double_height = 1;
								gotoxy(4, 1);
								lcdprint("    INVALID DATE    ");
								beep(ERRRBEEP);
								Delay_ms(2000);
								gotoxy(4, 1);
								lcdprint("                    ");
								count = start;
							}
						}
						else if(type == MONTH_YEAR)
						{
							if(check_for_valid_date(1, mm, yy))
							{
								temp_dd = 0;
								temp_mm = mm;
								temp_yy = yy;
								return 1;
							}
							else
							{
								clrscr();
								double_height = 1;
								gotoxy(4, 1);
								lcdprint("    INVALID DATE    ");
								beep(ERRRBEEP);
								Delay_ms(2000);
								gotoxy(4, 1);
								lcdprint("                    ");
								count = start;
							}
						}
						else if(type == HOUR_MIN)
						{
							if((mm <= 23) && (yy <= 59))
							{
								temp_dd = 0;
								temp_mm = mm;
								temp_yy = yy;
								return 1;
							}
							else
							{
								clrscr();
								double_height = 1;
								gotoxy(4, 1);
								lcdprint("    INVALID TIME    ");
								beep(ERRRBEEP);
								Delay_ms(2000);
								gotoxy(4, 1);
								lcdprint("                    ");
								count = start;
							}
						}
						break;
			default:
						if((key >= '0') && (key <= '9'))
						{
							if(count <= 7)
							{
								if((count == 2) || (count == 5))
									count++;
								date[count] = key;
								count++;
							}
						}
						break;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char check_for_valid_date(unsigned char d, unsigned char m, unsigned char y)
{
	UCHAR max = 0;

	clrscr();

	if(y==0)
	{
		return 0;
	}
	else
	{
		switch(m)
		{
//Check for months having maximum of 31 days
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
					max=31;
			break;

//Check for months having maximum of 30 days
			case 4:
			case 6:
			case 9:
			case 11:
					max=30;
			break;

//Check for februry month
			case 2:
			max = 28 + ((((y % 4)== 0) && ((y % 100) != 0))||((y % 400) == 0?1:0));
			break;

			default:
					return 0;
		}
	}

//Check whether the given date is valid or invalid
	if(d>0&&d<=max)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//-------------------------------------------------------------------------
void checkAppLockDate(void)
{
	UINT32 lockMins = 0, currMins = 0;
	if((mc.mc.lock_dd == 0) && (mc.mc.lock_mm == 0) && (mc.mc.lock_yy == 0))
	{

	}
	else
	{
		if(!mc.mc.app_lock)
		{
			lockMins = get_mins(mc.mc.lock_dd, mc.mc.lock_mm, mc.mc.lock_yy, 0, 0, 0);
			read_rtc();
			currMins = get_mins(tim.tim.dd, tim.tim.mm, tim.tim.yy, 0, 0, 1);

			if(currMins >= lockMins)
			{
				mc.mc.app_lock = 1;
				store_mc();
			}
		}
	}
}
//-------------------------------------------------------------------------
void updateTableMapContents(void)
{
	UINT16 loopCount = 0, i = 0, j = 0;

    memset(g_8K_membuf, 0, sizeof(g_8K_membuf));
	if(mc.mc.parcel_en_dis)
	{
		for(i = 0, loopCount = 0; i <= 250; i++)
		{
			for(j = 0; j < TABLE_MAP_LEN; j++)
				g_8K_membuf[loopCount++] = tablearray[i].arr[j];
		}
	}
	else
	{
		for(i = 0, loopCount = 0; i <= mc.mc.max_table; i++)
		{
			for(j = 0; j < TABLE_MAP_LEN; j++)
				g_8K_membuf[loopCount++] = tablearray[i].arr[j];
		}
	}
	FlashModifyData(((unsigned long)(TABLE_MAP_PG * PGBYTS)), g_8K_membuf , loopCount);  //bulk data transfered
	loadTableMapContents();
}
//-------------------------------vikas------------------------------------------
void loadTableMapContents(void)//RAM
{
	UINT16 i = 0;

	memset(tablearray, '\0', sizeof(tablearray));

	if(mc.mc.parcel_en_dis)
	{
		for(i = 0; i <= 250; i++)
		{
			get_table_map(i);
			tablearray[i].tablearray.table_array = tablemap.tablemap.table_map;
		}
	}
	else
	{
		for(i = 0; (i <= mc.mc.max_table); i++)
		{
			get_table_map(i);
			tablearray[i].tablearray.table_array = tablemap.tablemap.table_map;
		}
	}
}
//------------------------------vikas-------------------------------------------
unsigned int Map_udf_entry(unsigned int MapEntryIndex)
{
	UINT16 i = 0;

	if(mc.mc.parcel_en_dis)
	{
		for(i = 1; i <= 250 ; i++)
		{
			if(MapEntryIndex == tablearray[i].tablearray.table_array)
			{
					MapEntryIndex = i;
					return (MapEntryIndex) ;
			}
		}
	}
	else
	{
		for(i = 1; i <= mc.mc.max_table ; i++)
		{
			if(MapEntryIndex == tablearray[i].tablearray.table_array)
			{
					MapEntryIndex = i;
					return (MapEntryIndex) ;
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char displayQuantity(unsigned long printQty, unsigned char unitFract)
{
	UCHAR i = 0, space_cnt = 0;

	hex_asc(printQty, asc, (mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot));
	rem_zeros(asc,mc.mc.digitsBeforeDot - 1);
	space_cnt = rem_zeros_frac(asc,mc.mc.digitsAfterDot,(mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot));
	if(space_cnt)
	{
		for(i = 0;i < mc.mc.digitsBeforeDot;i++)
			disp_char(asc[i]);
	}
	else
	{
		for(i = 0;i < (mc.mc.digitsBeforeDot + mc.mc.digitsAfterDot);i++)
		{
			if(i == mc.mc.digitsBeforeDot)
				disp_char('.');
			disp_char(asc[i]);
		}
	}
	return space_cnt;
}
//-------------------------------------------------------------------------
unsigned char optionGSTSelection(void)
{
	UCHAR key = 0, exit = 0;
	clrscr();
	double_height = 1;
	double_width = 0;
	gotoxy(1,1);
	lcdprint("   SELECT GST TYPE  ");
	gotoxy(3,1);
	lcdprint("1. SGST & CGST      ");
	gotoxy(5,1);
	lcdprint("2. IGST             ");
	double_height = 0;
	gotoxy(8,1);
	lcdprint("PRESS        1/2/CNL");

	while(!exit)
	{
		while(!(key = keyPressCheck()));

		switch(key)
		{
			case '1':
						exit = 1;
						return 0;
			case '2':
						exit = 1;
						return 1;
			case '*':
						exit = 1;
						return 0;
			default :
						break;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
unsigned char checkMissedCallNumber(void)
{
	get_mobile_details();

	if(!strncmp(mob_details.mob_details.mobile1, mobileNumber, MOBILE_NUMBER_LEN))
		return 1;
	else if(!strncmp(mob_details.mob_details.mobile2, mobileNumber, MOBILE_NUMBER_LEN))
		return 2;
	else if(!strncmp(mob_details.mob_details.mobile3, mobileNumber, MOBILE_NUMBER_LEN))
		return 3;
	return 0;
}
//-------------------------------------------------------------------------
void getAndCheckMissedCalls(void)
{
//	UINT16 missedCallCount = 0, loop = 0;
//	UCHAR ret = 0;
//	UCHAR dateCurrent[25], dateSaved[25];
//
//	clrscr();
//	double_height = 1;
//	gotoxy(3,1);
//	lcdprint("CHECKING CALL DETAIL");
//	double_height = 0;
//	gotoxy(5,1);
//	lcdprint("   PLEASE WAIT...   ");
//	Delay_ms(1000);
//
//	missedCallCount = getMissedCallCount();
//
//	getMissedCallDetails(1);
//	ret = checkMissedCallNumber();
//	if(ret)
//	{
//		clrscr();
//		double_height = 1;
//		gotoxy(3,1);
//		lcdprint("   SENDING SUMMARY  ");
//		double_height = 0;
//		gotoxy(5,1);
//		lcdprint("   PLEASE WAIT...   ");
//		beep(SHRTBEEP);
//		Delay_ms(1000);
//
//		memset(membuf, '\0', sizeof(membuf));
//		memset(dateSaved, '\0', sizeof(dateSaved));
//		memset(dateCurrent, '\0', sizeof(dateCurrent));
//		switch(ret)
//		{
//			case 1:
//					get_call_details1(membuf);
//					break;
//			case 2:
//					get_call_details2(membuf);
//					break;
//			case 3:
//					get_call_details3(membuf);
//					break;
//		}
//		if((extractDateTimeFromCPBRCommand("\"\",\"",modem_rx_buff, dateCurrent)))
//		{
//			extractDateTimeFromCPBRCommand("\"\",\"", membuf, dateSaved);
//			if(strlen(dateCurrent))
//			{
//				if(strcmp(dateCurrent, dateSaved) != 0)
//				{
//					sendSMSDayWiseSummary();
//					getMissedCallDetails(1);
//					switch(ret)
//					{
//						case 1:
//								store_call_details1(modem_rx_buff);
//								break;
//						case 2:
//								store_call_details2(modem_rx_buff);
//								break;
//						case 3:
//								store_call_details3(modem_rx_buff);
//								break;
//					}
//				}
//			}
//		}
//	}
//	callReceived = 0;
//	clrscr();
}
//-----------------------------------------------------------------------------------
unsigned char extractDateTimeFromCPBRCommand(const char * searchStr, unsigned char * fromBuf, unsigned char * buf)
{
	char * resp;
	unsigned int i = 0;

	resp = strstr(fromBuf, searchStr);
	if(resp)
	{
		for(i = 4; i < strlen(resp); i++)
		{
			buf[i - 4] = *(resp + i);
			if(*((resp + i) + 1) == '"')
				break;
		}
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------
char sendCustomSMS(char * msg, char * mobile)
{
	unsigned char msgBuf[300] = {0};
	memset(msgBuf, 0, sizeof(msgBuf));

//	if(modem_connectivity == FALSE)
//	{
//		clrscr();
//		double_height = 1;
//		gotoxy(3,1);
//		lcdprint(" COMMUNICATION ERROR");
//		beep(ERRRBEEP);
//		Delay_ms(2000);
//		return ;
//	}
	stopPostCMDToModem();
	R_IWDT_Restart();
	strcpy(msgBuf, "SMS:SEND_SMS  :MOB:+91");
	strcat(msgBuf, mobile);
	strcat(msgBuf, "|:MSG:");
	strcat(msgBuf, msg);
	strcat(msgBuf, "|\r\n");
	initReception(1);
	SendModem(msgBuf);
	modemTimeOut = 1;
	R_IWDT_Restart();
	while((!modem_rx_done) && (modemTimeOut < 15000))
	{
		if((modemTimeOut % 5000) == 0)
			R_IWDT_Restart();
	}

	if(strstr(modem_rx_buff, "OK"))
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint("      SMS SENT      ");
		beep(OKKKBEEP);
		Delay_ms(2000);

		mc.mc.gsm_interval_data = 0;
		store_mc();
		return 1;
	}
	else
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint("    SMS NOT SENT    ");
		beep(OKKKBEEP);
		Delay_ms(2000);
		return 0;
	}
}
//-------------------------------------------------------------------------
void sendVoidStatus(unsigned long totalBills, unsigned long long totalAmount)
{
	unsigned char i = 0, j = 0, buff[200];

	FlashReadData((((UINT32) HEADER_PG * PGBYTS) + ((UINT32) 1 * HDR_LEN)), membuf, HDR_LEN);
	memset(buff, 0, 200);
	for(i = HDR_NM, j = 0; i < (HDR_NM + HDR_NM_LEN); i++)
	{
		if((membuf[i] != ' '))
		{
			buff[j++] = membuf[i];
		}
	}
	strcat(buff, ";BILLS VOIDED; BILLS : ");
	hex_asc(totalBills, asc, 5);
	rem_zeros(asc, 4);
	strcat(buff, asc);
	strcat(buff, " ;AMOUNT : ");
	value_hex_asc_64U = totalAmount;
	hex_asc_64U(membuf, 12);
	rem_zeros(membuf, 8);
	for(i = 0, j = 0; i < 12; i++)
	{
		if(i == 10)
			asc[j++] = '.';

		if(membuf[i] >= '0' && membuf[i] <= '9')
			asc[j++] = membuf[i];
	}
	asc[j] = 0;
	strcat(buff, asc);

	get_mobile_details();
	if(mob_details.mob_details.mobile1_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB1");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile1);
	}
	if(mob_details.mob_details.mobile2_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB2");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile2);
	}
	if(mob_details.mob_details.mobile3_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB3");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile3);
	}
}
//-------------------------------------------------------------------------
void sendItemReportStatus(void)
{
	unsigned char i = 0, j = 0, buff[200];

	FlashReadData((((UINT32) HEADER_PG * PGBYTS) + ((UINT32) 1 * HDR_LEN)), membuf, HDR_LEN);
	memset(buff, 0, 200);
	for(i = HDR_NM, j = 0; i < (HDR_NM + HDR_NM_LEN); i++)
	{
		if((membuf[i] != ' '))
		{
			buff[j++] = membuf[i];
		}
	}
	strcat(buff, ";ITEM WISE SALE REPORT PRINTED; BILLS : ");
	if(daily.daily.daily_set_var == 'Y')
		hex_asc(daily.daily.dailyBillNo, asc, 5);
	else
		hex_asc(0, asc, 5);
	rem_zeros(asc, 4);
	strcat(buff, asc);
	strcat(buff, "; AMOUNT : ");
	if(daily.daily.daily_set_var == 'Y')
		value_hex_asc_64U = daily.daily.dailyTotAmnt;
	else
		value_hex_asc_64U = 0;
	hex_asc_64U(membuf, 12);
	rem_zeros(membuf, 8);
	for(i = 0, j = 0; i < 12; i++)
	{
		if(i == 10)
			asc[j++] = '.';

		if(membuf[i] >= '0' && membuf[i] <= '9')
			asc[j++] = membuf[i];
	}
	asc[j] = 0;
	strcat(buff, asc);

	get_mobile_details();
	if(mob_details.mob_details.mobile1_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB1");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile1);
	}
	if(mob_details.mob_details.mobile2_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB2");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile2);
	}
	if(mob_details.mob_details.mobile3_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB3");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile3);
	}
}
//-------------------------------------------------------------------------
void sendEraseStatus(unsigned char eraseType, unsigned long totalBills, unsigned long long totalAmount)
{
	unsigned char i = 0, j = 0, buff[200];

	FlashReadData((((UINT32) HEADER_PG * PGBYTS) + ((UINT32) 1 * HDR_LEN)), membuf, HDR_LEN);
	memset(buff, 0, 200);
	for(i = HDR_NM, j = 0; i < (HDR_NM + HDR_NM_LEN); i++)
	{
		if((membuf[i] != ' '))
		{
			buff[j++] = membuf[i];
		}
	}
	if(eraseType == 1)
		strcat(buff, ";RESTORE DEFAULT DONE; BILLS : ");
	else
		strcat(buff, ";ERASE MASTER DONE; BILLS : ");
	hex_asc(totalBills, asc, 5);
	rem_zeros(asc, 4);
	strcat(buff, asc);
	strcat(buff, "; AMOUNT : ");
	value_hex_asc_64U = totalAmount;
	hex_asc_64U(membuf, 12);
	rem_zeros(membuf, 8);
	for(i = 0, j = 0; i < 12; i++)
	{
		if(i == 10)
			asc[j++] = '.';
		if(membuf[i] >= '0' && membuf[i] <= '9')
			asc[j++] = membuf[i];
	}
	asc[j] = 0;
	strcat(buff, asc);

	get_mobile_details();
	if(mob_details.mob_details.mobile1_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB1");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile1);
	}
	if(mob_details.mob_details.mobile2_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB2");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile2);
	}
	if(mob_details.mob_details.mobile3_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB3");
		Delay_ms(1000);
		sendCustomSMS(buff, mob_details.mob_details.mobile3);
	}
}
//-------------------------------------------------------------------------
void manageTAX366DaySummaryAndStore(unsigned char addFlag)
{
	UINT16 loop = 0, loopCount = 0, i = 0;

	for(i = 0; i < TWR_UNION_LEN; i++)
		twr.arr[i] = 0;
	get_twr(twr_ptr.twr_ptr.day_ptr);

	for(loopCount = 0; loopCount < recArrayPtr; loopCount++)
	{
		for(i = 0; i < RECDAT_LEN; i++)
			temprec.arr[i] = recArray[loopCount][i];

		if(addFlag == 1)
		{
			if(flagIGST)
			{
				if(temprec.temprec.prd_cgst == 500 )
				{
					twr.twr.igst_5_amt += temprec.temprec.cgst_amt;
					twr.twr.igst_5 = 500;
				}
				else if(temprec.temprec.prd_cgst == 1200 )
				{
					twr.twr.igst_12_amt += temprec.temprec.cgst_amt;
					twr.twr.igst_12 = 1200;
				}
				else if(temprec.temprec.prd_cgst == 1800 )
				{
					twr.twr.igst_18_amt += temprec.temprec.cgst_amt;
					twr.twr.igst_18 = 1800;
				}
				else if(temprec.temprec.prd_cgst == 2800 )
				{
					twr.twr.igst_28_amt += temprec.temprec.cgst_amt;
					twr.twr.igst_28 = 2800;
				}
				store_twr(twr_ptr.twr_ptr.day_ptr);
			}
			else
			{
				if(temprec.temprec.prd_cgst == 250 || temprec.temprec.prd_sgst == 250)
				{
					twr.twr.gst_5_amt += temprec.temprec.cgst_amt;
					twr.twr.gst_5 = 250;
				}
				else if(temprec.temprec.prd_cgst == 600 || temprec.temprec.prd_sgst == 600)
				{
					twr.twr.gst_12_amt += temprec.temprec.cgst_amt;
					twr.twr.gst_12 = 600;
				}
				else if(temprec.temprec.prd_cgst == 900 || temprec.temprec.prd_sgst == 900 )
				{
					twr.twr.gst_18_amt += temprec.temprec.cgst_amt;
					twr.twr.gst_18 = 900;
				}
				else if(temprec.temprec.prd_cgst == 1400 || temprec.temprec.prd_sgst == 1400)
				{
					twr.twr.gst_28_amt += temprec.temprec.cgst_amt;
					twr.twr.gst_28 = 1400;
				}
				store_twr(twr_ptr.twr_ptr.day_ptr);
			}
		}
		else
		{
			if(flagIGST)
			{
				if(temprec.temprec.prd_cgst == 500 )
				{
					twr.twr.igst_5_amt -= temprec.temprec.cgst_amt;
					twr.twr.igst_5 = 500;
				}
				else if(temprec.temprec.prd_cgst == 1200 )
				{
					twr.twr.igst_12_amt -= temprec.temprec.cgst_amt;
					twr.twr.igst_12 = 1200;
				}
				else if(temprec.temprec.prd_cgst == 1800 )
				{
					twr.twr.igst_18_amt -= temprec.temprec.cgst_amt;
					twr.twr.igst_18 = 1800;
				}
				else if(temprec.temprec.prd_cgst == 2800 )
				{
					twr.twr.igst_28_amt -= temprec.temprec.cgst_amt;
					twr.twr.igst_28 = 2800;
				}
				store_twr(twr_ptr.twr_ptr.day_ptr);
			}
			else
			{
				if(temprec.temprec.prd_cgst == 250 || temprec.temprec.prd_sgst == 250)
				{
					twr.twr.gst_5_amt -= temprec.temprec.cgst_amt;
					twr.twr.gst_5 = 250;
				}
				else if(temprec.temprec.prd_cgst == 600 || temprec.temprec.prd_sgst == 600)
				{
					twr.twr.gst_12_amt -= temprec.temprec.cgst_amt;
					twr.twr.gst_12 = 600;
				}
				else if(temprec.temprec.prd_cgst == 900 || temprec.temprec.prd_sgst == 900 )
				{
					twr.twr.gst_18_amt -= temprec.temprec.cgst_amt;
					twr.twr.gst_18 = 900;
				}
				else if(temprec.temprec.prd_cgst == 1400 || temprec.temprec.prd_sgst == 1400)
				{
					twr.twr.gst_28_amt -= temprec.temprec.cgst_amt;
					twr.twr.gst_28 = 1400;
				}
				store_twr(twr_ptr.twr_ptr.day_ptr);
			}
		}
	}
}
//-------------------------------------------------------------------------
void sendSMSBillWise_SMSToAllMobileNumbers(UINT32 Lastrec)
{
	get_mobile_details();
	if(mob_details.mob_details.mobile1_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB1");
		Delay_ms(1000);
		strncpy(mobileNumber, mob_details.mob_details.mobile1, 10);
		sendSMSBillWise(Lastrec);
	}
	if(mob_details.mob_details.mobile2_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB2");
		Delay_ms(1000);
		strncpy(mobileNumber, mob_details.mob_details.mobile2, 10);
		sendSMSBillWise(Lastrec);
	}
	if(mob_details.mob_details.mobile3_set == 'Y')
	{
		clrscr();
		double_height = 1;
		gotoxy(3,1);
		lcdprint(" SENDING SMS TO MOB3");
		Delay_ms(1000);
		strncpy(mobileNumber, mob_details.mob_details.mobile3, 10);
		sendSMSBillWise(Lastrec);
	}
}
//-------------------------------------------------------------------------
void sendSMSBillWise(UINT32 Lastrec)
{
	char buff[200], i = 0, j = 0;

	get_gsm_gprs();
//	if(modem_connectivity == FALSE)
//	{
//		clrscr();
//		double_height = 1;
//		gotoxy(3,1);
//		lcdprint(" COMMUNICATION ERROR");
//		beep(ERRRBEEP);
//		Delay_ms(2000);
//		mc.mc.checkIntervalFail = 0;
//		store_mc();
//		return;
//	}

	get_rec(Lastrec);

	if(rec.rec.round_off_tot_amt)
	{
		FlashReadData((((UINT32) HEADER_PG * PGBYTS) + ((UINT32) 1 * HDR_LEN)), membuf, HDR_LEN);
		memset(buff, 0, 200);
		for(i = HDR_NM, j = 0; i < (HDR_NM + HDR_NM_LEN); i++)
		{
			if((membuf[i] != ' '))
			{
				buff[j++] = membuf[i];
			}
		}
		strcat(buff, "; BILL NO:");
		hex_asc(rec.rec.rcpt_no, asc, 5);
		rem_zeros(asc, 4);
		strcat(buff, asc);

		pk_unpk(rec.rec.dd, dd2);
		pk_unpk(rec.rec.mm, mm2);
		pk_unpk(rec.rec.yy, yy2);
		pk_unpk(rec.rec.hr, hr2);
		pk_unpk(rec.rec.mn, mn2);
		pk_unpk(rec.rec.sc, sc2);

		strcat(buff, "; DT:");
		j = 0;
		asc[j++] = dd2[0];
		asc[j++] = dd2[1];
		asc[j++] = '/';
		asc[j++] = mm2[0];
		asc[j++] = mm2[1];
		asc[j++] = '/';
		asc[j++] = yy2[0];
		asc[j++] = yy2[1];

		asc[j] = 0;
		strcat(buff, asc);

		strcat(buff, "; TM:");
		j = 0;
		asc[j++] = hr2[0];
		asc[j++] = hr2[1];
		asc[j++] = ':';
		asc[j++] = mn2[0];
		asc[j++] = mn2[1];
		asc[j++] = ':';
		asc[j++] = sc2[0];
		asc[j++] = sc2[1];

		asc[j] = 0;
		strcat(buff, asc);

		strcat(buff, "; BILL AMOUNT:");
		value_hex_asc_64U = rec.rec.round_off_tot_amt;
		hex_asc_64U(membuf, 12);
		rem_zeros(membuf, 8);
		for(i = 0, j = 0; i < 12; i++)
		{
			if(i == 10)
				asc[j++] = '.';

			if(membuf[i] >= '0' && membuf[i] <= '9')
				asc[j++] = membuf[i];
		}
		asc[j] = 0;
		strcat(buff, asc);

		if(rec.rec.discount_amt)
		{
			strcat(buff, "; DISC:");
			hex_asc(rec.rec.discount_amt, membuf, 9);
			rem_zeros(membuf, 5);
			for(i = 0, j = 0; i < 9; i++)
			{
				if(i == 7)
					asc[j++] = '.';

				if(membuf[i] >= '0' && membuf[i] <= '9')
					asc[j++] = membuf[i];
			}
			asc[j] = 0;
			strcat(buff, asc);
		}

		if((rec.rec.sgst_amt+rec.rec.cgst_amt))
		{
			strcat(buff, "; TAX:");
			hex_asc((rec.rec.sgst_amt+rec.rec.cgst_amt), membuf, 9);
			rem_zeros(membuf, 5);
			for(i = 0, j = 0; i < 9; i++)
			{
				if(i == 7)
					asc[j++] = '.';

				if(membuf[i] >= '0' && membuf[i] <= '9')
					asc[j++] = membuf[i];
			}
			asc[j] = 0;
			strcat(buff, asc);
		}

		strcat(buff, "; TODAY'S SUMMARY:");
		value_hex_asc_64U = daily.daily.dailyTotAmnt;
		hex_asc_64U(membuf, 12);
		rem_zeros(membuf, 8);
		for(i = 0, j = 0; i < 12; i++)
		{
			if(i == 10)
				asc[j++] = '.';

			if(membuf[i] >= '0' && membuf[i] <= '9')
				asc[j++] = membuf[i];
		}
		asc[j] = 0;
		strcat(buff, asc);

		sendCustomSMS(buff,mobileNumber);
		clrscr();
	}
}
//-------------------------------------------------------------------------
void swap(unsigned int *a,unsigned  int *b,unsigned  int *a1,unsigned  int*b1)
{
  int t = *a;
  *a = *b;
  *b = t;

  int t1 = *a1;
  *a1 = *b1;
  *b1 = t1;
}
//-------------------------------------------------------------------------
int partition(unsigned int array[],unsigned int array1[],unsigned  int low,unsigned  int high)
{
  int pivot = array[high];
  int i = (low - 1);

  for (int j = low; j < high; j++)
  {
    if (array[j] <= pivot)
    {
      i++;
      swap(&array[i], &array[j],&array1[i], &array1[j]);
    }
  }
  swap(&array[i + 1], &array[high],&array1[i + 1], &array1[high]);
  return (i + 1);
}
//-------------------------------------------------------------------------
void quickSort(unsigned int array[],unsigned int array1[],unsigned  int low,unsigned  int high)
{
  if (low < high)
  {
    int pi = partition(array,array1, low, high);
    quickSort(array,array1, low, pi - 1);
    quickSort(array,array1, pi + 1, high);
  }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

