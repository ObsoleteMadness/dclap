Content-type: biocompute/command

# clustal.command
# d.gilbert, dec'97
# rewrite of clustal.html (seqpup) and clustalw.config (seqlab)
# as general parseable structure for specifying app parameters
# for use w/ a corba client-server interface

# magic string for this format is "Content-type: biocompute/command"
# format 
#  key = value
#  key = { structured.value }
#  newlines or ';' separate key=value pairs in a structure
#  values that include white space need to be quoted with "" or ''
#
# see bopper.idl and ReadCommand.java for current key words  (these may change)
# key words match fields in the bopper.idl, and are case-insensitive 
#
# commandKeys = { "id", "transport", "action", "filepath", "parlist", "command" };
# parameterKeys = { "id", "label", "value", "ifelseRules", "runSwitch" };
# containerKeys = { "required", "parlist" };
# choiceKeys = { "multiple", "minToShow", "parlist" };
# dataKeys = { "datatype", "dataflow", "filename", "flavor", "data" };
#
# ID values are case-sensitive, unique strings.
# reference IDs and other variables as $ID 
# $serverurl, $tempdir, and $tempname are variables that are set 
#   by server (remote or local) at runtime
#
# TITLE, INFO and HELP are special par ids 
#
# For MacOS, there is limited support for AppleScript commands
# when using the MRJ java runtime
# use the word applescript as the action command:
#    action = "applescript  text of script to run here"
# Currently no objects are returned, but script results are printed to System.out
#


command = {

id = clustalw
menu = "Sequence Alignment|Clustal multiple alignment"

#serverurl =
filepath = data/methods

## server only config -------
transport = local:
#transport = bop:$IORnumber
#transport = http:$serverurl

action= "$filepath/clustalw \
 $INFILE  $OUTFILE $ALIGN $TREE $QUICK $BOOT \
 $GAPEXT $GAPOPEN $200 $220 $100 $PAIRGAP $KTUP \
 $TOPDIAGS $WINDOW $PWGAPOPEN $PWGAPEXT $PWMATRIX \
 $300 $221 $211 $212 $213"
## ----------------------

parlist = {
par = {
	id = TITLE
	value.title = "Clustal W Alignment"
	}
	
par = {
	id = INFO
	label = "About Clustal W"
	// use .title instead of .string to flag non-edittable parameter !?
	value.title = "Clustal W - for multiple sequence alignment \
by Des Higgins and colleages. Clustal W is a general purpose multiple \
alignment program for DNA or proteins."
	}

par = {
	id = globalrules
	#<if "$INFILE.seq-type == protein">
  value.rules = -300,+200,+210,+220,+221,+211,+212,+213
  #<else>
  value.rules = +300,-200,-210,-220,-221,-211,-212,-213
  #<endif>
}

par = {
	id = main
  label = "Clustal W - A multiple sequence alignment program"

	value.container = {
		required = true
		parlist = {
		par = {
			id = HELP
			label = "Help with Clustal W"
			value.url = file://$filepath/clustalw_help
			}
		
		par = {
	    id = ALIGN
	    label = "Do full multiple align"
	    value.boolean = true
	    runSwitch = -align
	  	}	
	  }
	  }
	  
}

par = {
	id = IOfiles
	label = "Input/Output files"
	value.container = {
		required = false
		parlist = {
		
		par = {
			## this is messy -- client has data in some form (file, array, etc)
			## this par should only specify kind=input-sequence, type=bioseq/...
	    id = INFILE
	    label = "Input sequences"
	    value.data = {
	    	data = 
	      dataflow = input
	      datatype = biosequence/nbrf
	    	filename = clustalw.pir
	    	##filename = $tempname
	    	flavor = input
	    	}
	    runSwitch = "-infile=$value"
	  	}

		par = {
	    id = OUTFILE
	    label = "Output aligned sequences"
	    value.data = {
	    	data = 
	      dataflow = output
	      datatype = biosequence/gcg
	    	filename = clustalw.msf
	    	##filename = $temp.outseq
	    	flavor = output
	    	}
	    runSwitch = "-outfile=$value -output=GCG"
	  	}

		par={ id= TreeFile; label="Tree file";
			value.data={dataflow= output; datatype= biotree/newick; flavor= output;
			filename= clustalw.dnd;}
			}

		par={ id= STDOUT; label="Command output";
			value.data={dataflow= output; datatype= text/plain; flavor= stdout;
			filename= clustalw-out.text;}
			}

		par={ id= STDERR; label="Errors";
			value.data={dataflow= output;datatype= text/plain; flavor= stderr;
			filename= clustalw.err;}
			}
		}
	}
}

par = {
	id = treeoptions
	label = "Tree options"
	
	value.container = {
		required = false
		parlist = {

		par = {
			id= TREE
			label= "Calculate NJ tree"
			value.boolean = false
			runSwitch = -tree
			}

		par = {
			id= BOOT
			label= "Bootstrap NJ tree"
			value.boolean = false
			runSwitch = "-bootstrap=$BOOTVAL"
			}
			
		par = {
			id= BOOTVAL
			label= "No. of boostraps"
			value.integer = 1000
			}
		}
		}
}


par = {
	id = pairoptions
	label = "Pairwise alignment options"
	value.container = {
		required = false
		parlist = {
		
			par = {
				id= 100
				value.choice = {
					multipleChoices = false
					minToShow = 2
					parlist = {
					par = {
						id = QUICK
						label= "Perform fast, approximate pair-wise alignments"
						value.boolean= false
						runSwitch = -quicktree
						ifelseRules= +KTUP,+WINDOW,+PAIRGAP,+TOPDIAGS,-PWMATRIX,-USERDEF
						}
					par = {
						id = SLOW
						label= "Perform slow, accurate pair-wise alignments"
						value.boolean= true
						runSwitch = -pairwise
				    #<if "$INFILE.seq-type == protein">
						ifelseRules= "-KTUP,-WINDOW,-PAIRGAP,-TOPDIAGS,+PWMATRIX,+USERDEF"
					  #<else>
						ifelseRules= "-KTUP,-WINDOW,-PAIRGAP,-TOPDIAGS,-PWMATRIX,-USERDEF"
					  #<endif>
						}
					}
				}
				}
		
			par = {
				id= PAIRGAP
		    label = "Gap penalty"
				#<if "$INFILE.seq-type == protein">
				value.intRange= 3,1,500,1
				#<else>
				value.intRange= 5,1,50,1
				#<endif>
				runSwitch= "-pairgap=$value"
				}		

			par = {
				id= KTUP
		    label = "Wordsize (ktuple)"
				#<if "$INFILE.seq-type == protein">
				value.intRange= 1,1,2,1
				#<else>
				value.intRange= 2,1,4,1
				#<endif>
				runSwitch= "-ktuple=$value"
				}		
				
			par = {
				id= TOPDIAGS
		    label = "Number of best diagonals"
				#<if "$INFILE.seq-type == protein">
				value.intRange= 5,1,50,1
				#<else>
				value.intRange= 4,1,50,1
				#<endif>
				runSwitch= "-topdiags=$value"
				}		
				
			par = {
				id= WINDOW
		    label= "Window size"
			  #<if "$INFILE.seq-type == protein">
				value.intRange= 5,1,50,1
				#<else>
				value.intRange= 4,1,50,1
				#<endif>
				runSwitch= "-window=$value"
				}		

			par = {
				id= PWGAPOPEN
		    label= "Window Gap penalty"
				value.floatRange= 10.0,0,100,0.1
				runSwitch= "-pwgapopen=$value"
				}		

			par = {
				id= PWGAPEXT
		    label= "Window Gap extension"
			  #<if "$INFILE.seq-type == protein">
				value.floatRange= 0.1,0,10,0.1
				#<else>
				value.floatRange= 5.0,0,10,0.1
				#<endif>
				runSwitch= "-pwgapext=$value"
				}		

			par = {
				id= PWMATRIX
		    label = "Pair-wise scoring matrix"
				value.choice = {
					multipleChoices = false
					minToShow = 6
					parlist = { 
					par = {
						id = 1
						label= BLOSUM30
						value.boolean = true
						runSwitch= "-pwmatrix=blosum"
						}
					par = {
						id = 2
						label= PAM350
						value.boolean = false
						runSwitch= "-pwmatrix=pam"
						}
					par = {
						id = 3
						label= Gonnet250
						value.boolean = false
						runSwitch= "-pwmatrix=gonnet"
						}
					par = {
						id = 4
						label= "Identity Matrix"
						value.boolean = false
						runSwitch= "-pwmatrix=id"
						}
					par = {
						id = 5
						label= "User Defined"
						value.boolean = false
						runSwitch= "-matrix=$USERDEF"
						}
					par = {
						id = USERDEF
			  		label = "Scoring Matrix file name"
						value.string=
						}
					}
				}
				}
				
		}
		}
}

par = {
	id = multalign
	label = "Multiple sequence alignment options"
	value.container = {
		required = false
		parlist = {
		par = {
			id= GAPOPEN
	    label= "Gap opening penalty"
			value.floatRange= 10.0,0,100,1.0
			runSwitch= "-gapopen=$value"
			}		
			
		par = {
			id= GAPEXT
	    label= "Gap extension penalty"
			#<if "$INFILE.seq-type == protein">
			value.floatRange= 0.05,0,10,0.1
			#<else>
			value.floatRange= 5.0,0,10,0.1
			#<endif>
			runSwitch= "-gapext=$value"
			}		
			
		par = {
			id = 300
			label = "Do NOT Weight Transitions"
			value.boolean = false
			runSwitch= -transitions
			}

		par = {
			id= 220
	    label= "Delay divergent sequences"
			value.intRange= 40,0,100,1
			runSwitch= "-maxdiv=$value"
			}		

		par = {
			id = 221
			label = "Use a negative matrix"
			value.boolean = false
			runSwitch= -negative
			}
			
		par = {
			id= 200
	    label = "Protein scoring matrices"
			value.choice = {
				multipleChoices = false
				parlist = {
				par = {
					id = 6
					label= BLOSUM
					value.boolean = true
					runSwitch= "-matrix=blosum"
					}
				par = {
					id = 7
					label= "Identity Matrix"
					value.boolean = false
					runSwitch= "-matrix=id"
					}
				par = {
					id = 8
					label= PAM
					value.boolean = false
					runSwitch= "-matrix=pam"
					}
				par = {
					id = 9
					label= "User Defined"
					value.boolean = false
					runSwitch= "-matrix=$210"
					}
				par = {
					id = 210
		  		label = "Scoring Matrix file name"
					value.string=
					}
				}
			}
			}
			
		
		par = {
			id = 211
			label = "Turn Off Residue-Specific Gap Penalties"
			value.boolean = false
			runSwitch= -nopgap
			}
			
		par = {
			id = 212
			label = "Turn Off Hydrophilic Gap Penalties"
			value.boolean = false
			runSwitch= -nohgap
			}

		par = {
			id = 213
  		label = "List of Hydrophilic Residues"
			value.string= "GPSNDQEKR"
			runSwitch= "-hgapresidues=$value"
  		## ?? what are these
  		#Toggle = True
  		#Toggledefault = -
			}
					
		}
		}
}

} # end parameters
} # end command
