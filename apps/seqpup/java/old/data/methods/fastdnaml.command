Content-type: biocompute/command

# fastdnaml.command
# d.gilbert, jan'98
# child app command for use with seqpup [java]

command = {

id = fastdnaml
menu = "Phylogeny|FastDNAml"
filepath = data/methods
transport = local:

action= "$filepath/fastdnaml $TREEFILE "

parlist = {
par = {
	id = TITLE
	value.title = "FastDNAml"
	}
	
par = {
	id = INFO
	label = "About FastDNAml"
	value.title = "A faster DNA maximum likelihood phylogeny estimator.\
 fastDNAml is a program derived from Joseph Felsenstein's version 3.3 DNAML\
 (part of his PHYLIP package).  Users should consult the documentation for\
 DNAML before using this program.\
 Produced by Gary Olsen, Hideo Matsuda, Ray Hagstrom, and Ross Overbeek."
	}

par = {
	id = main
  label = "fastDNAml phylogeny analysis"

	value.container = {
		required = true
		parlist = {
		par = {
			id = HELP
			label = "Help with FastDNAml"
			value.url = file://$filepath/fastDNAml-help.txt
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

		## !! need way to stuff parameters into data files (stdin)
		## phylip (others?) take all params from top of data input file
		
		par={ id= STDIN; label="Input";
			value.data={dataflow= input; datatype= biosequence/phylip; flavor= stdin;
			filename= fastdnaml.infile;}
			}
		
		par={ id= STDOUT; label="Output";
			value.data={dataflow= output; datatype= text/plain; flavor= stdout;
			filename= fastdnaml.outfile; }
			}
			##??
		par={ id= TREEFILE; label="Tree file";
			value.data={dataflow= output; datatype= biotree/newick; flavor= output;
			filename= "$filepath/treefile.0"; }
			}

		par={ id= STDERR; label="Errors";
			value.data={dataflow= output;datatype= text/plain; flavor= stderr;
			filename= fastdnaml.err;}
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

		## !! need way to stuff parameters into data files (stdin)
		## phylip (others?) take all params from top of data input file
		par = {
			id = optNotice
			label = "NOTICE"
			value.title = "These option selections are not yet implemented in the local versions of seqpup"
			}

		par = {
			id = bootstrap
			#?? runSwitch = "echo $first_line B; echo B $bootseed;"
			value.boolean = false
			label = "bootstrap"
			}
		par = {
			id = bootseed
			#value.randint
			value.integer = 987
			label = "bootstrap seed"
			}
			
		par = {
			id = categories
			#?? runSwitch = "echo $first_line C; echo C $catnum $catlist;"
			value.boolean = false
			label = "use categories"
			}
		par = {
			id = catnum
			value.integer = 1
			label = "Number of categories"
			}
		par = {
			id = catlist
			value.string =  
			label = "category values for each site"
			}
		
		par = {
			id = frequencies
			#?? runSwitch = "echo $first_line F; echo F $freqlist;"
			value.boolean = false
			label = "use frequencies"
			}
		par = {
			id = freqlist
			value.string = "0.24 0.28 0.27 0.21"
			label = "empirical base frequencies (A,C,G,T)"
			}
		
		par = {
			id = global
			#?? runSwitch = "echo $first_line G; echo G $Global1 $Global2;"
			value.boolean = false
			label = "Global rearrangements"
			}
		par = {
			id = Global1
			value.string = ""
			label = "Global1: the number of branches to cross in rearrangements of the completed tree"
			}
		par = {
			id = Global2
			value.string = ""
			label = "Global2: the number of branches to cross in testing rearrangements"
			}
		
		par = {
			id = jumble
			#?? runSwitch = "echo $first_line J; echo J $jumbleseed;"
			value.boolean = false
			label = "Jumble"
			}
		par = {
			id = jumbleseed
			#value.randint
			value.integer = 987 
			label = "jumble seed"
			}

		par = {
			id = outgroup
			#?? runSwitch = "echo $first_line O; echo O $outval;"
			value.boolean = false
			label = "use outgroup"
			}
		par = {
			id = outval
			value.integer = 1
			label = "outgroup number"
			}

		par = {
			id = quickadd
			#?? runSwitch = "echo $first_line Q; "
			value.boolean = false
			label = "use Quickadd option"
			}
		
		par = {
			id = transitions
			#?? runSwitch = "echo $first_line T; echo T $transval;"
			value.boolean = false
			label = "use transitions/transversion ratio"
			}
		par = {
			id = transval
			value.float = 2.0
			label = "transition/transversion ratio"
			}

	## usertree
	## userlengths
		
		par = {
			id = weights
			#?? runSwitch = "echo $first_line W; echo $weightlist;"
			value.boolean = false
			label = "use weights"
			}
		par = {
			id = weightlist
			value.string = ""
			label = "weighting values per site"
			}

		}
	}
}

} # end parameters
} # end command
