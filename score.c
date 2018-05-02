// 
//  Font data for Elephant 13pt
// 
#include "score.h"

// Character bitmaps for Elephant 13pt
const uint8_t numbersBitmaps[] = 
{
		// @0 '0' (13 pixels wide)
	0x00, 0x00, //               
	0x1F, 0x80, //    ######     
	0x3F, 0xC0, //   ########    
	0x7F, 0xE0, //  ##########   
	0xFF, 0xF0, // ############  
	0xC0, 0x30, // ##        ##  
	0x80, 0x10, // #          #  
	0xC0, 0x30, // ##        ##  
	0xFF, 0xF0, // ############  
	0x7F, 0xE0, //  ##########   
	0x3F, 0xC0, //   ########    
	0x1F, 0x80, //    ######     
	0x00, 0x00, //               

	// @26 '1' (13 pixels wide)
	0x00, 0x00, //               
	0x00, 0x00, //               
	0x00, 0x10, //            #  
	0x00, 0x10, //            #  
	0xFF, 0xF0, // ############  
	0xFF, 0xF0, // ############  
	0xFF, 0xF0, // ############  
	0xFF, 0xF0, // ############  
	0x80, 0x10, // #          #  
	0x80, 0x10, // #          #  
	0x80, 0x10, // #          #  
	0x00, 0x00, //               
	0x00, 0x00, //               

	// @52 '2' (13 pixels wide)
	0x00, 0x00, //               
	0x01, 0x00, //        #      
	0x39, 0xE0, //   ###  ####   
	0x7E, 0xF0, //  ###### ####  
	0xFE, 0x70, // #######  ###  
	0xFF, 0x70, // ######## ###  
	0xC3, 0x70, // ##    ## ###  
	0x9D, 0xF0, // #  ### #####  
	0x9C, 0xF0, // #  ###  ####  
	0xFC, 0x70, // ######   ###  
	0x7C, 0x30, //  #####    ##  
	0x38, 0x10, //   ###      #  
	0x00, 0x00, //               

	// @78 '3' (13 pixels wide)
	0x01, 0x80, //        ##     
	0x73, 0xC0, //  ###  ####    
	0x7F, 0xE0, //  ##########   
	0xFF, 0xE0, // ###########   
	0xFF, 0xF0, // ############  
	0xFF, 0xF0, // ############  
	0xC6, 0x30, // ##   ##   ##  
	0xB4, 0x90, // # ## #  #  #  
	0xB5, 0xD0, // # ## # ### #  
	0xF5, 0xF0, // #### # #####  
	0x71, 0xF0, //  ###   #####  
	0x31, 0xE0, //   ##   ####   
	0x00, 0xC0, //         ##    

	// @104 '4' (13 pixels wide)
	0x00, 0x50, //          # #  
	0x00, 0x50, //          # #  
	0xFF, 0xF0, // ############  
	0xFF, 0xF0, // ############  
	0x7F, 0xF0, //  ###########  
	0x3F, 0xF0, //   ##########  
	0x0C, 0x50, //     ##   # #  
	0x06, 0x50, //      ##  # #  
	0x03, 0x40, //       ## #    
	0x01, 0xC0, //        ###    
	0x00, 0xC0, //         ##    
	0x00, 0x00, //               
	0x00, 0x00, //               

	// @130 '5' (13 pixels wide)
	0x00, 0x00, //               
	0x03, 0xC0, //       ####    
	0xC7, 0xE0, // ##   ######   
	0xCF, 0xE0, // ##  #######   
	0x6F, 0xF0, //  ## ########  
	0x6C, 0x30, //  ## ##    ##  
	0x68, 0xD0, //  ## #   ## #  
	0xE8, 0xF0, // ### #   ####  
	0xED, 0xF0, // ### ## #####  
	0x3F, 0xE0, //   #########   
	0x1E, 0xE0, //    #### ###   
	0x00, 0x00, //               
	0x00, 0x00, //               

	// @156 '6' (13 pixels wide)
	0x00, 0x00, //               
	0x63, 0xC0, //  ##   ####    
	0xEF, 0xE0, // ### #######   
	0xEF, 0xF0, // ### ########  
	0xEF, 0xF0, // ### ########  
	0x88, 0x10, // #   #      #  
	0xCC, 0x10, // ##  ##     #  
	0xFF, 0xF0, // ############  
	0x7F, 0xE0, //  ##########   
	0x3F, 0xE0, //   #########   
	0x1F, 0x80, //    ######     
	0x00, 0x00, //               
	0x00, 0x00, //               

	// @182 '7' (13 pixels wide)
	0x00, 0x00, //               
	0xC0, 0x00, // ##            
	0xE0, 0x00, // ###           
	0xF0, 0x00, // ####          
	0xF8, 0x00, // #####         
	0xEE, 0x00, // ### ###       
	0xE7, 0xF8, // ###  ######## 
	0xE1, 0xFC, // ###    #######
	0xE0, 0xFC, // ###     ######
	0xE0, 0x7C, // ###      #####
	0x70, 0x38, //  ###      ### 
	0x10, 0x00, //    #          
	0x00, 0x00, //               

	// @208 '8' (13 pixels wide)
	0x00, 0x00, //               
	0x73, 0xC0, //  ###  ####    
	0x7F, 0xE0, //  ##########   
	0xCF, 0xF0, // ##  ########  
	0x8F, 0xF0, // #   ########  
	0x8F, 0x10, // #   ####   #  
	0x9F, 0x10, // #  #####   #  
	0xFE, 0x10, // #######    #  
	0x7F, 0x30, //  #######  ##  
	0x7D, 0xE0, //  ##### ####   
	0x19, 0xC0, //    ##  ###    
	0x00, 0x00, //               
	0x00, 0x00, //               

	// @234 '9' (13 pixels wide)
	0x00, 0x00, //               
	0x1F, 0x00, //    #####      
	0x7F, 0xC0, //  #########    
	0x7F, 0xE0, //  ##########   
	0xFF, 0xF0, // ############  
	0xC1, 0x30, // ##     #  ##  
	0x81, 0x10, // #      #   #  
	0xFF, 0x70, // ######## ###  
	0xFF, 0xF0, // ############  
	0x7F, 0xF0, //  ###########  
	0x3C, 0x60, //   ####   ##   
	0x00, 0x00, //               
	0x00, 0x00, //   
};

// Character descriptors for Elephant 13pt
// { [Char width in bits], [Offset into elephant_13ptCharBitmaps in bytes] }
const uint16_t numbersDescriptors[] = 
{
	0, 		// 0 
	26, 		// 1 
	52, 		// 2 
	78, 		// 3 
	104, 		// 4 
	130, 		// 5 
	156, 		// 6 
	182, 		// 7 
	208, 		// 8 
	234, 		// 9 
};
