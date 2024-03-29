#include "Types.r"
#include "SysTypes.r"
 
data 'ics8' (128) {
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FF00 0500 0500 0500 0500 0500 0500 05FF"            /* ................ */
	$"FF05 FF05 0005 FF05 0005 FFFF FF05 00FF"            /* ................ */
	$"FF00 FFFF 0500 FF00 05FF 0500 05FF 05FF"            /* ................ */
	$"FF05 FF05 FF05 FF05 00FF 0005 0005 00FF"            /* ................ */
	$"FF00 FF00 05FF FF00 05FF 0500 05FF 05FF"            /* ................ */
	$"FF05 FF05 0005 FF05 0005 FFFF FF05 00FF"            /* ................ */
	$"FF00 0500 0500 0500 0500 0500 0500 05FF"            /* ................ */
	$"FF05 0005 0005 0005 0005 0005 0005 00FF"            /* ................ */
	$"FF00 FFFF FFFF 0500 05FF FFFF FFFF 00FF"            /* ................ */
	$"FF05 FF05 0005 FF05 0005 00FF 0500 05FF"            /* ................ */
	$"FF00 FFFF FFFF 0500 0500 05FF 0005 00FF"            /* ................ */
	$"FF05 FF05 0005 FF05 0005 00FF 0500 05FF"            /* ................ */
	$"FF00 FFFF FFFF 0500 05FF FFFF FFFF 00FF"            /* ................ */
	$"FF05 0005 0005 0005 0000 0500 0500 05FF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'ics4' (128) {
	$"FFFF FFFF FFFF FFFF F010 1010 1010 101F"            /* ................ */
	$"F1F1 01F1 01FF F10F F0FF 10F0 1F10 1F1F"            /* ................ */
	$"F1F1 F1F1 0F01 010F F0F0 1FF0 1F10 1F1F"            /* ................ */
	$"F1F1 01F1 01FF F10F F010 1010 1010 101F"            /* ................ */
	$"F101 0101 0101 010F F0FF FF10 1FFF FF0F"            /* ................ */
	$"F1F1 01F1 010F 101F F0FF FF10 101F 010F"            /* ................ */
	$"F1F1 01F1 010F 101F F0FF FF10 1FFF FF0F"            /* ................ */
	$"F101 0101 0010 101F FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'ics#' (128) {
	$"FFFF 8001 A239 B245 AA41 A645 A239 8001"            /* ...’9²EͺA¦E’9. */
	$"8001 BC7D A211 BC11 A211 BC7D 8001 FFFF"            /* .Ό}’.Ό.’.Ό}... */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'ICN#' (128) {
	$"FFFF FFFF 8000 0001 B18F 3F3D B199 9999"            /* .......±?=± */
	$"B9B1 9999 BDB0 1999 BFB0 1F19 B7B0 1999"            /* Ή±½°.Ώ°..·°. */
	$"B3B1 9999 B199 9999 B18F 3F3D 8000 0001"            /* ³±±±?=... */
	$"FFFF FFFF 8000 0001 9F1F 1F7D 9111 1145"            /* .........}..E */
	$"88A8 A289 88A8 A289 8444 4001 8444 5D5D"            /* ¨’¨’D@.D]] */
	$"822A 3555 822A 3555 8515 1555 8515 1555"            /* *5U*5U..U..U */
	$"8888 8D5D 8888 8801 9145 4501 9145 4501"            /* ].EE.EE. */
	$"A222 2201 BE3E 3E01 8000 0001 FFFF FFFF"            /* ’"".Ύ>>........ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'icl8' (128) {
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FF00 0500 0500 0500 0500 0500 0500 0500"            /* ................ */
	$"0500 0500 0500 0500 0500 0500 0500 05FF"            /* ................ */
	$"FF05 FFFF 0005 00FF FF05 0005 FFFF FFFF"            /* ................ */
	$"0005 FFFF FFFF FFFF 0005 FFFF FFFF 00FF"            /* ................ */
	$"FF00 FFFF 0500 05FF FF00 05FF FF00 05FF"            /* ................ */
	$"FF00 05FF FF00 05FF FF00 05FF FF00 05FF"            /* ................ */
	$"FF05 FFFF FF05 00FF FF05 FFFF 0005 00FF"            /* ................ */
	$"FF05 00FF FF05 00FF FF05 00FF FF05 00FF"            /* ................ */
	$"FF00 FFFF FFFF 05FF FF00 FFFF 0500 0500"            /* ................ */
	$"0500 05FF FF00 05FF FF00 05FF FF00 05FF"            /* ................ */
	$"FF05 FFFF FFFF FFFF FF05 FFFF 0005 0005"            /* ................ */
	$"0005 00FF FFFF FFFF 0005 00FF FF05 00FF"            /* ................ */
	$"FF00 FFFF 05FF FFFF FF00 FFFF 0500 0500"            /* ................ */
	$"0500 05FF FF00 05FF FF00 05FF FF00 05FF"            /* ................ */
	$"FF05 FFFF 0005 FFFF FF05 FFFF 0005 00FF"            /* ................ */
	$"FF05 00FF FF05 00FF FF05 00FF FF05 00FF"            /* ................ */
	$"FF00 FFFF 0500 05FF FF00 05FF FF00 05FF"            /* ................ */
	$"FF00 05FF FF00 05FF FF00 05FF FF00 05FF"            /* ................ */
	$"FF05 FFFF 0005 00FF FF05 0005 FFFF FFFF"            /* ................ */
	$"0005 FFFF FFFF FFFF 0005 FFFF FFFF 00FF"            /* ................ */
	$"FF00 0500 0500 0500 0500 0500 0500 0500"            /* ................ */
	$"0500 0500 0500 0500 0500 0500 0500 05FF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FF00 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 00FF"            /* ................ */
	$"FF00 00FF FFFF FFFF 0000 00FF FFFF FFFF"            /* ................ */
	$"0000 00FF FFFF FFFF 00FF FFFF FFFF 00FF"            /* ................ */
	$"FF00 00FF 0005 00FF 0000 00FF 0005 00FF"            /* ................ */
	$"0000 00FF F9F9 F9FF 00FF F9F9 F9FF 00FF"            /* ................ */
	$"FF00 0000 FF00 0500 FF00 FFF9 FF00 0500"            /* ................ */
	$"FF00 FFF9 F9F9 FF00 FFF9 F9F9 FF00 00FF"            /* ................ */
	$"FF00 0000 FF05 0005 FF00 FFF9 FF05 0005"            /* ................ */
	$"FF00 FFF9 F9F9 FF00 FFF9 F9F9 FF00 00FF"            /* ................ */
	$"FF00 0000 00FF 0500 05FF F9F9 F9FF 0500"            /* ................ */
	$"00FF F9F9 F9F9 0000 F9F9 F9F9 0000 00FF"            /* ................ */
	$"FF00 0000 00FF 0005 00FF F9F9 F9FF 0005"            /* ................ */
	$"00FF F9FF FFFF 00FF F9FF F9FF FFFF 00FF"            /* ................ */
	$"FF00 0000 0000 FF00 0500 FFF9 FF00 FF00"            /* ................ */
	$"0500 FFFF F9FF 00FF F9FF F9FF 00FF 00FF"            /* ................ */
	$"FF00 0000 0000 FF05 0005 FFF9 FF00 FF05"            /* ................ */
	$"0005 FFFF 00FF 00FF F9FF F9FF 00FF 00FF"            /* ................ */
	$"FF00 0000 00FF F9FF 0500 05FF 00FF F9FF"            /* ................ */
	$"0500 05FF 00FF F9FF F9FF 00FF 00FF 00FF"            /* ................ */
	$"FF00 0000 00FF F9FF 0005 00FF 00FF F9FF"            /* ................ */
	$"0005 00FF 00FF F9FF F9FF 00FF 00FF 00FF"            /* ................ */
	$"FF00 0000 FFF9 F9F9 FF00 0500 FFF9 F9F9"            /* ................ */
	$"FF00 0500 FFFF F9FF F9FF 00FF FFFF 00FF"            /* ................ */
	$"FF00 0000 FFF9 F9F9 FF05 0005 FFF9 F9F9"            /* ................ */
	$"FF05 0005 FFF9 F9F9 F900 0000 0000 00FF"            /* ................ */
	$"FF00 00FF F9F9 F9FF 00FF 0500 05FF F9FF"            /* ................ */
	$"00FF 0500 05FF F9FF 0000 0000 0000 00FF"            /* ................ */
	$"FF00 00FF F9F9 F9FF 00FF 0005 00FF F9FF"            /* ................ */
	$"00FF 0005 00FF F9FF 0000 0000 0000 00FF"            /* ................ */
	$"FF00 FFF9 F9F9 FF00 0000 FF00 0500 FF00"            /* ................ */
	$"0000 FF00 0500 FF00 0000 0000 0000 00FF"            /* ................ */
	$"FF00 FFFF FFFF FF00 0000 FFFF FFFF FF00"            /* ................ */
	$"0000 FFFF FFFF FF00 0000 0000 0000 00FF"            /* ................ */
	$"FF00 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 00FF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'icl4' (128) {
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"F010 1010 1010 1010 1010 1010 1010 101F"            /* ................ */
	$"F1FF 010F F101 FFFF 01FF FFFF 01FF FF0F"            /* ................ */
	$"F0FF 101F F01F F01F F01F F01F F01F F01F"            /* ................ */
	$"F1FF F10F F1FF 010F F10F F10F F10F F10F"            /* ................ */
	$"F0FF FF1F F0FF 1010 101F F01F F01F F01F"            /* ................ */
	$"F1FF FFFF F1FF 0101 010F FFFF 010F F10F"            /* ................ */
	$"F0FF 1FFF F0FF 1010 101F F01F F01F F01F"            /* ................ */
	$"F1FF 01FF F1FF 010F F10F F10F F10F F10F"            /* ................ */
	$"F0FF 101F F01F F01F F01F F01F F01F F01F"            /* ................ */
	$"F1FF 010F F101 FFFF 01FF FFFF 01FF FF0F"            /* ................ */
	$"F010 1010 1010 1010 1010 1010 1010 101F"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"F000 0000 0000 0000 0000 0000 0000 000F"            /* ................ */
	$"F00F FFFF 000F FFFF 000F FFFF 0FFF FF0F"            /* ................ */
	$"F00F 010F 000F 010F 000F DDDF 0FDD DF0F"            /* ................ */
	$"F000 F010 F0FD F010 F0FD DDF0 FDDD F00F"            /* ................ */
	$"F000 F101 F0FD F101 F0FD DDF0 FDDD F00F"            /* ................ */
	$"F000 0F10 1FDD DF10 0FDD DD00 DDDD 000F"            /* ................ */
	$"F000 0F01 0FDD DF01 0FDF FF0F DFDF FF0F"            /* ................ */
	$"F000 00F0 10FD F0F0 10FF DF0F DFDF 0F0F"            /* ................ */
	$"F000 00F1 01FD F0F1 01FF 0F0F DFDF 0F0F"            /* ................ */
	$"F000 0FDF 101F 0FDF 101F 0FDF DF0F 0F0F"            /* ................ */
	$"F000 0FDF 010F 0FDF 010F 0FDF DF0F 0F0F"            /* ................ */
	$"F000 FDDD F010 FDDD F010 FFDF DF0F FF0F"            /* ................ */
	$"F000 FDDD F101 FDDD F101 FDDD D000 000F"            /* ............Π... */
	$"F00F DDDF 0F10 1FDF 0F10 1FDF 0000 000F"            /* ................ */
	$"F00F DDDF 0F01 0FDF 0F01 0FDF 0000 000F"            /* ................ */
	$"F0FD DDF0 00F0 10F0 00F0 10F0 0000 000F"            /* ................ */
	$"F0FF FFF0 00FF FFF0 00FF FFF0 0000 000F"            /* ................ */
	$"F000 0000 0000 0000 0000 0000 0000 000F"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'BNDL' (128) {
	$"454E 545A 0000 0001 4652 4546 0000 0000"            /* ENTZ....FREF.... */
	$"0080 4943 4E23 0000 0000 0080"                      /* .ICN#..... */
};

data 'ENTZ' (0, "Owner resource") {
	$"1245 6E74 7265 7A20 5265 6C65 6173 6520"            /* .Entrez Release  */
	$"332E 30"                                            /* 3.0 */
};

data 'FREF' (128) {
	$"4150 504C 0000 00"                                  /* APPL... */
};

