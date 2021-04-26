Content-type: biocompute/command

# tacg.command
# d.gilbert, dec'97
# for use of Harry Mangalam tacg restriction mapping program
#
# this is a hasty, not final, selection of tagc command options
#


command = {

id = tacg
menu = "Restriction Map|tacg"

filepath = data/methods
datapath = data/prefs

## server only config -------
transport = local:
#transport = iiop://chipmunk.bio.indiana.edu:7000/Bopper 

action= "$filepath/tacg $REBASE $orderout $REselect $circular $linear \
$codon.u $codon.v $codon.d $codon.sc $codon.sp $codon.nc $codon.hp $codon.c \
$fragunsort $fragsort $frag3 $gelmap $laddermap $linearmap $summary $sites "

# -R $filepath/rebase.data

## ----------------------

parlist = {
par = {
	id = TITLE
	value.title = "tacg restriction map"
	}

par = {
	id = HELP
	label = "Help with tacg"
	#value.url = http://chipmunk.bio.indiana.edu/bopper/help/tacg2.main.html
	value.url = file://$filepath/tacg2.main.html
	}
	
par = {
	id = INFO
	label = "About tacg"
	value.title = "tacg analyzes a DNA sequence for restriction enzyme sites, \
nucleic acid patterns version 2, by Harry Mangalam"
	}


par = {
	id = IOfiles
	label = "Input/Output files"
	value.container = {
		required = false
		parlist = {
		
		par={ id= STDIN; label="Input";
			value.data={dataflow= input; datatype= biosequence/fasta; flavor= stdin;
			filename= tacg.inseq;}
			}

		par={ id= STDOUT; label="Output";
			value.data={dataflow= output; datatype= text/plain; flavor= stdout;
			filename= tacg.map;}
			}

		## not getting filepath/datapath set right !!!
		par={ id= REBASE; label="Restrict. Enzyme table";
			value.data={dataflow= output; datatype= text/gcg; flavor= serverlocal;
			filename= "$filepath/rebase.data"; }
			runSwitch= "-R $value"
			}


		par={ id= STDERR; label="Errors";
			value.data={dataflow= output;datatype= text/plain; flavor= stderr;
			filename= tacg.err;}
			}
		}
	}
}


par = {
	id = Options
	label = "Options"
	value.container = {
		required = false
		parlist = {

		par = {
			id = orderout
			runSwitch = "-c"
			value.boolean = false
			label = "order the output by # of cuts/fragments by each RE, and thence alphabetic"
			}

		par = {
			id = laddermap
			runSwitch = "-l"
			value.boolean = false
			label = "a 'ladder' map of selected enzymes"
			}
			
		par = {
			id = linearmap
			runSwitch = "-L"
			value.boolean = false
			label = "if you WANT a linear map. This spews the most output"
 			}
			
		par = {
			id = summary
			runSwitch = "-s"
			value.boolean = false
			label = "print the summary of site information"
 			}

		par = {
			id = sites
			runSwitch = "-S"
			value.boolean = false
			label = "print the the actual cut sites in tabular form"
 			}
 			
		par = {
			id = 98
			label = "Print/Sort Fragments"
			value.choice = {
				multipleChoices = false
				parlist = {
					par = { id= 988; label=Omit; value.boolean=true;  }
					par = { id= fragunsort; label="Print unsorted"; value.boolean=false; runSwitch = "-F1"; }
					par = { id= fragsort; label="Print sorted"; value.boolean=false; runSwitch = "-F2"; }
					par = { id= frag3; label="Print sorted & unsorted"; value.boolean=false; runSwitch = "-F3"; }
					}
				}
			}

		par = {
			id = 96
			label = "pseudo-gel map graphic"
			value.container = {
				required = false
				parlist = {
					par= { id= gelmap; label="do map"; value.boolean= false; runSwitch = "-g$gelmapval"; }
					par= { id= gelmapval; label="map val"; value.int= 10; }
					} }
			}

		par = {
			id = 99
			label = "form (or topology) of DNA "
			value.choice = {
				multipleChoices = false
				parlist = {
					par = { id= circular; label=Circular; value.boolean=false; runSwitch = "-f0"; }
					par = { id= linear; label=Linear; value.boolean=true; runSwitch = "-f1"; }
					}
				}
			}
									
			par = {
				id= 100
				label= "Codon usage"
				value.choice = {
					multipleChoices = false
					parlist = {
					par = { id = codon.u; label= "Universal"; value.boolean= true; runSwitch = -C0; }
					par = { id = codon.v; label= "Mito.Vertebrates"; value.boolean= false; runSwitch = -C1; }
					par = { id = codon.d; label= "Mito.Drosophila "; value.boolean= false; runSwitch = -C2; }
					par = { id = codon.sc; label= "Mito.S.Cervisiae "; value.boolean= false; runSwitch = -C3 }
					par = { id = codon.sp; label= "Mito.S.Pombe "; value.boolean= false; runSwitch = -C4; }
					par = { id = codon.nc; label= "Mito.N.crassa "; value.boolean= false; runSwitch = -C5; }
					par = { id = codon.hp; label= "Mito.Higher.Plants "; value.boolean= false; runSwitch = -C6; }
					par = { id = codon.c; label= "Mito.Ciliates "; value.boolean= false; runSwitch = -C7; }
					}
				}
				}
	}
	} }
				
par= {
	id = REnzymes
	label = "Restriction Enzymes"
	value.container = {
	  required = false
	  parlist = {
	  	par = {
	  		id = REselect
	  		label = "Select specific r. enzymes"
	  		runSwitch = "-r $RElist"
	  		value.boolean = false
	  		}
	  	par = {
	  		id = RElist
	  		label = "List of enzymes to select"
	  		value.list = "AatII|AccI|AceIII|AciI|AclI|AflII|AflIII|AhdI|AluI|AlwI|AlwNI|\
ApaI|ApaLI|ApoI|AscI|AvaI|AvaII|AvrII|BaeI|BaeI|BamHI|\
BanI|BanII|BbsI|BbvI|BccI|Bce83I|BcefI|BcgI|BcgI|BciVI|\
BclI|BfaI|BfiI|BglI|BglII|BmgI|BplI|BpmI|Bpu10I|Bpu1102I|\
BsaI|BsaAI|BsaBI|BsaHI|BsaJI|BsaWI|BsaXI|BsbI|BscGI|BseMII|\
BseRI|BsgI|BsiEI|BsiHKAI|BslI|BsmI|BsmAI|BsmBI|BsmFI|Bsp24I|\
Bsp24I|Bsp1286I|BspEI|BspGI|BspLU11I|BspMI|BsrI|BsrBI|BsrDI|BsrFI|\
BsrGI|BssHII|BssSI|Bst4CI|BstAPI|BstDSI|BstEII|BstXI|BstYI|BstZ17I|\
Bsu36I|Cac8I|CjeI|CjeI|CjePI|CjePI|ClaI|CviJI|CviRI|DdeI|\
DpnI|DraI|DraIII|DrdI|DrdII|EaeI|EagI|EarI|EciI|Eco47III|\
Eco57I|EcoNI|EcoO109I|EcoRI|EcoRII|EcoRV|FauI|Fnu4HI|FokI|FseI|\
FspI|GdiII|HaeI|HaeII|HaeIII|HgaI|HgiEII|HhaI|Hin4I|HincII|HindIII|HinfI|HpaI|HphI|KpnI|\
MaeIII|MboII|MluI|MmeI|MnlI|MscI|MseI|MslI|MspI|MspA1I|\
MunI|MwoI|NarI|NciI|NcoI|NdeI|NgoAIV|NheI|NlaIII|NlaIV|\
NotI|NruI|NsiI|NspI|NspV|PacI|Pfl1108I|PflMI|PinAI|PleI|\
PmeI|PmlI|PshAI|Psp5II|PstI|PvuI|PvuII|RcaI|RleAI|RsaI|\
RsrII|SacI|SacII|SalI|SanDI|SapI|Sau96I|Sau3AI|SbfI|ScaI|\
ScrFI|SexAI|SfaNI|SfcI|SfiI|SgfI|SgrAI|SimI|SmaI|SmiI|\
SmlI|SnaBI|SpeI|SphI|SrfI|Sse8647I|SspI|StuI|StyI|SunI|\
TaiI|TaqI|TaqII|TaqII|TatI|TauI|TfiI|ThaI|TseI|Tsp45I|\
Tsp509I|TspRI|Tth111I|Tth111II|UbaDI|UbaEI|VspI|XbaI|XcmI|XhoI|XmnI"
				}
			}
		}
}

				

} # end parameters
} # end command
