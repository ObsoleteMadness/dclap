Content-type: biocompute/command

# cap.command
# d.gilbert, jan'98
# child app command for use with seqpup [java]

command = {

id = cap
menu = "Sequence Alignment|CAP contig assembly"
filepath = data/methods
transport = local:
action= "$filepath/cap $INFILE $OUTFILE $MIN_OVERLAP $PERCENT_MATCH "

parlist = {
par = {
	id = TITLE
	value.title = "CAP Contig Assembly"
	}
	
par = {
	id = INFO
	label = "About CAP"
	value.title = " A gel Contig Assembly Program by Xiaoqiu Huang"
	}

par = {
	id = main
  label = "CAP contig assembly"

	value.container = {
		required = true
		parlist = {
		par = {
			id = HELP
			label = "Help with CAP"
			value.url = file://$filepath/cap-help.txt
			}

		par = {
			id = MIN_OVERLAP
			label= "Minimum base overlap for contig alignment"
			value.integer = 20
			runSwitch= $value
			}
			
		par = {
			id = PERCENT_MATCH
			label= "Percent match for alignment"
			#value.integer = 85
			value.intrange = 85,0,100,1
			runSwitch= $value
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
	    id = INFILE
	    label = "Input sequences"
	    value.data = {
	      dataflow = input
	    	flavor = input
	      datatype = biosequence/fasta
	    	filename = cap-in.fasta
	    	}
	    runSwitch = "$value"
	  	}

		par = {
	    id = OUTFILE
	    label = "Output aligned sequences"
	    value.data = {
	      dataflow = output
	    	flavor = output
	      datatype = biosequence/fasta
	    	filename = cap-out.fasta
	    	}
	    runSwitch = "$value"
	  	}

		par={ id= STDOUT; label="Command output";
			value.data={dataflow= output; datatype= text/plain; flavor= stdout;
			filename= cap-out.text;}
			}

		par={ id= STDERR; label="Errors";
			value.data={dataflow= output;datatype= text/plain; flavor= stderr;
			filename= cap.err;}
			}
		}
	}
}

} # end parameters
} # end command
