/*  subutil.h
* >> Set tabs to 4 spaces for a nice printout
*   
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* File Name:  subutil.h
*
* Author:  James Ostell
*   
* Version Creation Date: 11/3/93
*
* $Revision: 1.1 $
*
* File Description: Utilities for creating ASN.1 submissions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_SubUtil_
#define _NCBI_SubUtil_

#ifndef _NCBI_Submit_
#include <objsub.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
*
*   Create a GenBank direct submission
*   	This supports a basic set of datatypes for making a new direct
*   submission to GenBank in ASN.1. It is designed for folks wanting to
*   read their own data storage format, then make a valid direct submission
*   without going through an intermediate tool.
*
*   	You may have many "entries" in a single submission. A single entry
*   may contain:
*   	One protein sequence       (called a "Bioseq")
*   	One nucleic acid sequence  (called a "Bioseq")
*   	One nucleic acid sequence for which you only have a series of
*          sequence pieces (e.g. you sequenced around the exons of a
*          genomic sequence, but not the introns) (called a "segmented
*          set")
*   	One nucleic acid sequence and the protein sequences it codes for.
*          (nucleic acid may a a single Bioseq or a segmented set)
*          (this entry called a "nuc-prot set")
*
*   NCBI considers the protein sequences part
*   of the submission, and they are created as proteins in their own right
*   by the routines below. You can either supply the protein sequence from
*   your own software (best case), in which we check that the coding region
*   you supply translates to it. If you do not supply a protein sequence,
*   then all we can do is check that it translates without stops.
*
*   	NCBI also considers "gene" to refer to a region of nucleic acid
*   within which are found elements (such as promoters, coding regions,etc)
*   leading to a phenomenon recognized as a gene (note this also accomodates
*   anonymous markers as well as expressed products). This is in contrast to
*   so other notions that a gene is simply a qualifier on other features of
*   the DNA. A separate function to produce a gene feature is supplied. The
*   intervals given for it should include the intervals for the other
*   features it contains.
*
*   	The process of building the direct submission is roughly:
*
*   Create the submission
*   	Add the submission citation
*   	Create an entry  (can be 1 or more sequences)
*   		Add the organism information
*   		Add any publication citations
*   		Add the sequences
*             Fill in the residues
*   		Add the features
*   		Validate the entry
*   Write the entry
*   Free the memory used
*
*   Each element may have subfunctions:
*
*   Create a citation
*   	Add author names
*   	Add author affiliation
*
*   Create a sequence
*   	Add modifiers
*
*   Create a feature
*   	Add information specific to type of feature
*   	Add intervals on the sequence
*   
*****************************************************************************/
typedef Boolean (* SubErrorFunc) (CharPtr msg);

typedef struct ncbisub {
	SeqSubmitPtr ssp;               /* the submission */
	SubErrorFunc err_func;          /* the error handler */
	Int2 gap_count;                 /* for unique gap names in segs */
} NCBISub, PNTR NCBISubPtr;

#define PubPtr ValNodePtr           /* should really be typedeffed */

/*****************************************************************************
*
*   Prototypes for building a direct submission
*
*****************************************************************************/

						 /* default error handler */

Boolean DefaultSubErrorFunc (CharPtr msg);

/*****************************************************************************
*
*   Create/Free the NCBISub
*
*****************************************************************************/

NCBISubPtr NCBISubBuild (
	CharPtr name,
	CharPtr PNTR address ,
	CharPtr phone ,
	CharPtr fax ,
	CharPtr email,
	Boolean hold_until_publish ,
	Int2 release_month,
	Int2 release_day,
	Int2 release_year );

               /** every submission must have 1 submission citation **/
               /** see below to add authors and affiliation **********/

Boolean CitSubForSubmission (
	NCBISubPtr submission,
	PubPtr cit_sub );

Boolean NCBISubWrite (
	NCBISubPtr ssp,
	CharPtr filename );

NCBISubPtr NCBISubFree (
	NCBISubPtr ssp );

/*****************************************************************************
*
*   You can (should) run the ncbi validator routines on your final submission.
*   It returns a count of all errors or questions found.
*   You may pass in errfile to see the error messages. Normally this would
*   be stderr, but it could be any open FILE *. This function does not
*   open or close the error file.
*
*****************************************************************************/

Int2 NCBISubValidate (NCBISubPtr nsp, FILE * errfile);

/*****************************************************************************
*
*   Add Entries to the Submission
*   Add Sequences to the Entries
*
*****************************************************************************/

/*****************************************************************************
*
*   About Sequence Identifiers:
*
*   Note that in all functions below where you create a Bioseq in your entry,
*   you can supply a number of different pieces of information to make a
*   sequence id.
*
*   local_name: This is a string for whatever name you call this sequence
*               locally. Could be a clone name or whatever. There are no
*               limits on this other than it should be unique in the
*               submission. It is REQUIRED.
*
*   SeqEntryPtr: Returned by the function, this is a pointer to the Bioseq.
*
*   In later functions, such as adding feature locations, you can refer to
*   the Bioseq you created either with the local_name or directly with the
*   SeqEntryPtr. Whatever is more convenient for you is fine.
*
*   The other ids only apply to updates. These allow you to update your
*   entry in GenBank simply by sending a new entry with the same accession
*   number you were issued on the last one. In this case you should also
*   be sure to add the create_date, which will be returned to you in the
*   ASN.1 of your direct submission after processing. This is not absolutely
*   required, but does let us check that it is the right entry (errors
*   could occur when you enter your old accession number).
*
*   genbank_locus:  OPTIONAL on update. The name appearing on the LOCUS line.
*   genbank_accession: REQUIRED on update.
*   gi_number: OPTIONAL on update for now. The unique ID assigned by NCBI
*      to a particular sequence (DNA or protein) in your entry.
*
*   If you update your entry, whether you change the sequence or not, the
*   accession number and locus will remain the same, so people can retrieve
*   your new data with the old id. However, the gi_number is explicitly keyed
*   to the sequence, and will change if there are any changes/additions to
*   the sequence. In addition, a history will be created indicating the old
*   gi_number and the date the new entry replaced it. Both old and new
*   entries will be available from NCBI for retrieval on gi_number. Only the
*   new entry will appear in the next GenBank or Entrez release.
*
*****************************************************************************/


            /*** Entry contains only 1 Bioseq ***/

SeqEntryPtr AddSeqOnlyToSubmission (
	NCBISubPtr submission ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness );

            /*** Entry contains a segmented set of Bioseqs ***/

SeqEntryPtr AddSegmentedSeqToSubmission (
	NCBISubPtr submission ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness );

SeqEntryPtr AddSeqToSegmentedEntry (
	NCBISubPtr submission,
	SeqEntryPtr segmented_seq_entry,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness );

Boolean AddGapToSegmentedEntry (
	NCBISubPtr submission,
	SeqEntryPtr segmented_seq_entry,
	Int4 length_of_gap );    /** 0 if not known */

Boolean AddReferenceToSegmentedEntry (
	NCBISubPtr submission ,
	SeqEntryPtr segmented_seq_entry,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int4 from ,
	Int4 to ,
	Boolean on_plus_strand );


			/*** Entry contains nucleotide and translated proteins ***/

SeqEntryPtr AddNucProtToSubmission (
	NCBISubPtr submission );

SeqEntryPtr AddSeqToNucProtEntry (   /** add unsegmented nuc or prot bioseq */
	NCBISubPtr submission,
	SeqEntryPtr nuc_prot_entry,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness );
								  /** add segmented nuc or prot bioseq set */

SeqEntryPtr AddSegmentedSeqToNucProtEntry (
	NCBISubPtr submission,
	SeqEntryPtr nuc_prot_entry ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness );

#define MOLECULE_CLASS_DNA 1
#define MOLECULE_CLASS_RNA 2
#define MOLECULE_CLASS_NUC 4
#define MOLECULE_CLASS_PROTEIN 3

#define MOLECULE_TYPE_GENOMIC 1
#define MOLECULE_TYPE_PRE_MRNA 2
#define MOLECULE_TYPE_MRNA 3
#define MOLECULE_TYPE_RRNA 4
#define MOLECULE_TYPE_TRNA 5
#define MOLECULE_TYPE_SNRNA 6
#define MOLECULE_TYPE_SCRNA 7
#define MOLECULE_TYPE_PEPTIDE 8
#define MOLECULE_TYPE_OTHER_GENETIC_MATERIAL 9
#define MOLECULE_TYPE_GENOMIC_MRNA_MIX 10

#define TOPOLOGY_LINEAR 1
#define TOPOLOGY_CIRCULAR 2
#define TOPOLOGY_TANDEM 3

#define STRANDEDNESS_SINGLE 1
#define STRANDEDNESS_DOUBLE 2
           
		   /******************************************************************
		   *
		   *   Fill in Bases or Amino Acids
		   *   	 1) You may call functions as often per bioseq as you like
		   *   		  up to the length of the Bioseq
		   *   	 2) All codes are iupac and defined in /ncbi/data/seqcode.prt
		   *   		  as an ASN.1 file used by this code. Excerpts at the
		   *   		  end of this file. Even though it's ASN.1 you will find
		   *   		  you can read it with no trouble.
		   *   	 3) IUPAC codes are UPPER CASE. These functions will upper
		   *   		  case for you.
		   *   	 4) In nucleic acids 'U' will be changed to 'T'
		   *   	 5) In both cases, non-letters will be stripped from the
		   *   		  the input strings to facilate input from external
		   *   		  formatted files with numbers and internal spaces and
		   *          such.
		   *     
		   ******************************************************************/

Boolean AddBasesToBioseq (
	NCBISubPtr submission ,
	SeqEntryPtr the_seq ,
	CharPtr the_bases );

Boolean AddAminoAcidsToBioseq (
	NCBISubPtr submission ,
	SeqEntryPtr the_seq ,
	CharPtr the_aas );


/*****************************************************************************
*
*   Add Annotations to Entries
*
*****************************************************************************/

Boolean AddTitleToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr title );

			/*****************************************************************
			*
			*   rules for long comments
			*     1) include no non-ascii characters (e.g. \t \r \n)
			*     2) you may force a line feed on display by using tilde '~'
		    *     3) you format a table by adding leading spaces after a '~'
			*     4) non-ascii chars will be converted on input (also for
			*         title) \n='~', all others='#'
			*
			*****************************************************************/

Boolean AddCommentToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr comment );

Boolean AddOrganismToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr scientific_name ,
	CharPtr common_name ,
	CharPtr virus_name ,
	CharPtr strain ,
	CharPtr synonym1,
	CharPtr synonym2,
	CharPtr synonym3);

Boolean AddGenBankBlockToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr taxonomy ,
	CharPtr division ,
	CharPtr keyword1 ,
	CharPtr keyword2 ,
	CharPtr keyword3 );

Boolean AddCreateDateToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	Int2 month ,
	Int2 day ,
	Int2 year );

	/*************************************************************************
	*
	*   Modifiers modify the meaning of all entries in the set or sequence
	*   to which they are applied. This is particularly important for
	*   indicating organelle sequences, RNA genomes, or mutants.
	*
	*   Less obvious is indicating completness.
	*
	*   A genomic sequence is assumed to be partial unless the "complete"
	*      modifier is used.
	*   A peptide sequence is assumed to be complete unless the "partial"
	*      modifier is used.
	*   A cDNA is assumed to be complete (as well as one can tell) unless
	*      "partial" is used.
	*   
	*   A genomic sequence is assumed to be nuclear unless the "mitochondrial"
	*      (or other organelle) modifier is used.
	*   All sequences are assumed to be natural unless "synthetic",
	*      "recombinant", or "mutagen" are used.
	*
	*************************************************************************/


Boolean AddModifierToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	Int2 modifier );

#define MODIF_dna  0
#define MODIF_rna  1
#define MODIF_extrachrom  2
#define MODIF_plasmid  3
#define MODIF_mitochondrial  4
#define MODIF_chloroplast  5
#define MODIF_kinetoplast  6
#define MODIF_cyanelle  7
#define MODIF_synthetic  8		/* synthetic sequence */
#define MODIF_recombinant  9	/* recombinant construct */
#define MODIF_partial  10
#define MODIF_complete  11
#define MODIF_mutagen  12 /* subject of mutagenesis ? */
#define MODIF_natmut  13  /* natural mutant ? */
#define MODIF_transposon  14
#define MODIF_insertion_seq  15
#define MODIF_no_left  16 /* missing left end (5' for na, NH2 for aa) */
#define MODIF_no_right  17   /* missing right end (3' or COOH) */
#define MODIF_macronuclear  18
#define MODIF_proviral  19
#define MODIF_est  20    /* expressed sequence tag */


	                           /*** add/build publications ***/
Boolean AddPubToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	PubPtr pub );

PubPtr CitSubBuild (
	NCBISubPtr submission,
	Int2 month,
	Int2 day,
	Int2 year,
	Int2 medium );

#define MEDIUM_NOT_SET 0
#define MEDIUM_PAPER 1
#define MEDIUM_TAPE 2
#define MEDIUM_FLOPPY 3
#define MEDIUM_EMAIL 4
#define MEDIUM_OTHER 255

PubPtr CitArtBuild (
	NCBISubPtr submission,
	CharPtr title ,
	CharPtr journal ,
	CharPtr volume ,
	CharPtr issue ,
	CharPtr pages ,
	Int2 month ,
	Int2 day ,
	Int2 year ,
	Int2 status );

#define PUB_STATUS_PUBLISHED 0
#define PUB_STATUS_SUBMITTED 1
#define PUB_STATUS_IN_PRESS  2
#define PUB_STATUS_UNPUBLISHED 3

	/*************************************************************************
	*
	*   Author names can be given in various forms
	*   	You MUST give at least a last name
	*   	You should give at least first name or initials.
	*       Initials are just for first and middle names, and are
	*         separated by periods.
	*
	*   example: John Q. Public
	*   last_name = "Public"
	*   first_name = "John"
	*   middle_name = NULL
	*   initials = "J.Q."
	*
	*************************************************************************/


Boolean AddAuthorToPub (    /* call once for each author, in order */
	NCBISubPtr submission,
	PubPtr the_pub,
	CharPtr last_name,
	CharPtr first_name,
	CharPtr middle_name,
	CharPtr initials,  /* separated by periods, no initial for last name */
	CharPtr suffix );  /* Jr. Sr. III */


	/*************************************************************************
	*
	*   Author Affiliation
	*      only one allowed per pub (one per author is also possible, but is
	*      not supported by this interface )
	*
	*   affil = institutional affiliation
	*   div   = division of institution
	*   street = street address
	*   city = city
	*   sub = subdivision of country (e.g. state.. optional)
	*   country = country
	*
	*************************************************************************/


Boolean AddAffiliationToPub (  /* call once per pub */
	NCBISubPtr submission,
	PubPtr the_pub,
	CharPtr affil,        /* e.g. "Xyz University" */
	CharPtr div,          /* e.g. "Dept of Biology" */
	CharPtr street,       /* e.g. "123 Academic Road" */
	CharPtr city,         /* e.g. "Metropolis" */
	CharPtr sub,          /* e.g. "Massachusetts" */
	CharPtr country );    /* e.g. "USA" */


/*****************************************************************************
*
*   Add Features to the entry
*   	Add location to feature
*   	Add info for specific types to feature
*
*****************************************************************************/
SeqFeatPtr FeatureBuild (
	NCBISubPtr submission,
	SeqEntryPtr entry_to_put_feature,
	Boolean feature_is_partial,
	Uint1 evidence_is_experimental,
	Boolean biological_exception,
	CharPtr comment );

#define EVIDENCE_NOT_SET 0
#define EVIDENCE_EXPERIMENTAL 1
#define EVIDENCE_NOT_EXPERIMENTAL 2

	/*************************************************************************
	*
	*   About feature locations:
	*     Internally the NCBI software represents locations on sequence as
	*	offsets from the start of the sequence (i.e. from 0 - (length -1)).
	*   Also, the "from" position is always <= "to", even for locations on
	*   the minus strand. Finally, no location can cross the origin of a
	*   circular sequence.. it must be split in two. This makes routines
	*   that access locations very consistent and easy to write.
	*
	*     However, most biologists number sequences starting with 1, not 0.
	*   It is natural to think of a coding region on the minus strand going
	*   from 5243 to 2993. And it is not unusual to think of the origin of
	*   replication being from 5344 to 10 on the plus strand of a circular
	*   sequence.
	*
	*     AddIntervalToFeature and AddPointToFeature were written to support
	*   the biological notion. They convert to the internal format
	*   automatically. So, for these two functions:
	*
	*	1) numbers are in the range 1 - length
	*   2) from <= to on plus strand
	*      to <= from on minus strand
	*   3) numbers not conforming to (2) are assumed to go around the origin
	*      of a circular sequence. It is an error on a linear sequence.
	*   4) Intervals should be added in biological order (e.g. exon1, exon2,
	*      exon3...) no matter which strand the feature is on.
	*   5) You must always indicate explicitly the Bioseq the interval is
	*      on. You may either pass in the SeqEntryPtr or the local_name you
	*      used when you created the sequence. The sequence must have
	*      been previously created with AddSeqTo...	 If you give both the
	*      SeqEntryPtr and the local_name, they must agree.
	*   6) -1 (minus one) is a short hand for "end of sequence". To indicate
	*      the whole sequence you can give from = 1, to = -1
	*
	*************************************************************************/

Boolean AddIntervalToFeature (
	NCBISubPtr submission,
	SeqFeatPtr sfp,
	SeqEntryPtr the_seq ,
	CharPtr local_name ,
	Int4 from ,
	Int4 to ,
	Boolean on_plus_strand ,
	Boolean start_before_from ,
	Boolean stop_after_to );

Boolean AddPointToFeature (
	NCBISubPtr submission,
	SeqFeatPtr sfp,
	SeqEntryPtr the_seq ,
	CharPtr local_name ,
	Int4 location ,
	Boolean on_plus_strand ,
	Boolean is_after_location ,
	Boolean is_before_location );

	/*************************************************************************
	*
	*   Having made a generalized feature, now add type specific info to it.
	*
	*************************************************************************/
			
			/*****************************************************************
			*
			*   This connects a protein sequence with the nucleic acid
			*   region which codes for it. So the protein is given as an
			*   argument, as well as adding intervals on the nucleic acid.
			*   A complete coding region includes the initial Met codon and
			*   the final termination codon.
			*
			*****************************************************************/


Boolean MakeCdRegionFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	Int2 frame ,
	Int2 genetic_code ,			/* see end of this file for genetic codes */
	SeqEntryPtr protein_product ,	/* give id of protein. if NULL, call */
	CharPtr local_id_for_protein );  /* function below to create by transl */
           
		   /******************************************************************
		   *
		   *   Special function to make protein from CdRegion feature
		   *
		   ******************************************************************/

SeqEntryPtr TranslateCdRegion (
	NCBISubPtr submission ,
	SeqFeatPtr cdregion_feature ,
	SeqEntryPtr nuc_prot_entry_to_put_sequence ,
	CharPtr local_name ,             /* for protein sequence */
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number );

Boolean MakeRNAFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	Int2 rna_type ,
	Boolean is_pseudo_gene,
	CharPtr rna_name ,
	CharPtr AA_for_tRNA ,
	CharPtr codon_for_tRNA );

#define RNA_TYPE_premsg 1
#define RNA_TYPE_mRNA   2
#define RNA_TYPE_tRNA   3
#define RNA_TYPE_rRNA   4
#define RNA_TYPE_snRNA  5
#define RNA_TYPE_scRNA  6
#define RNA_TYPE_other  255

Boolean MakeGeneFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	CharPtr gene_symbol_for_locus ,
	CharPtr allele ,
	CharPtr descriptive_name ,
	CharPtr map_location ,
	Boolean is_pseudo_gene ,
	CharPtr genetic_database ,
	CharPtr gene_id_in_genetic_database ,
	CharPtr synonym1 ,
	CharPtr synonym2 ,
	CharPtr synonym3 );

Boolean MakeProteinFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	CharPtr protein_name1,
	CharPtr protein_name2,
	CharPtr protein_name3,
	CharPtr descriptive_name,
	CharPtr ECnum1,
	CharPtr ECnum2,
	CharPtr activity1,
	CharPtr activity2,
	CharPtr protein_database,
	CharPtr id_in_protein_database);

Boolean MakeRegionFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	CharPtr region_name );

Boolean MakeSiteFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	Int2 site_type );

Boolean MakeImpFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	CharPtr key );

Boolean AddQualToImpFeature (
	NCBISubPtr submission,
	SeqFeatPtr imp_feature ,
	CharPtr qualifier ,
	CharPtr value );

Boolean MakePubFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	PubPtr pub );

#ifdef __cplusplus
)
#endif

#endif


/*****************************************************************************
*
*   Allowed IUPAC nucleic acid codes from /ncbi/data/seqcode.prt
*   
                ( symbol "A", name "Adenine" ),
                ( symbol "B" , name "G or T or C" ),
                ( symbol "C", name "Cytosine" ),
                ( symbol "D", name "G or A or T" ),
                ( symbol "G", name "Guanine" ),
                ( symbol "H", name "A or C or T" ) ,
                ( symbol "K", name "G or T" ),
                ( symbol "M", name "A or C" ),
                ( symbol "N", name "A or G or C or T" ) ,
                ( symbol "R", name "G or A"),
                ( symbol "S", name "G or C"),
                ( symbol "T", name "Thymine"),
                ( symbol "V", name "G or C or A"),
                ( symbol "W", name "A or T" ),
                ( symbol "Y", name "T or C")
*   
*
*****************************************************************************/

/*****************************************************************************
*
*   Allowed IUPAC amino acid codes from /ncbi/data/seqcode.prt

                ( symbol "A", name "Alanine" ),
                ( symbol "B" , name "Asp or Asn" ),
                ( symbol "C", name "Cysteine" ),
                ( symbol "D", name "Aspartic Acid" ),
                ( symbol "E", name "Glutamic Acid" ),
                ( symbol "F", name "Phenylalanine" ),
                ( symbol "G", name "Glycine" ),
                ( symbol "H", name "Histidine" ) ,
                ( symbol "I", name "Isoleucine" ),
                ( symbol "K", name "Lysine" ),
                ( symbol "L", name "Leucine" ),
                ( symbol "M", name "Methionine" ),
                ( symbol "N", name "Asparagine" ) ,
                ( symbol "P", name "Proline" ),
                ( symbol "Q", name "Glutamine"),
                ( symbol "R", name "Arginine"),
                ( symbol "S", name "Serine"),
                ( symbol "T", name "Threoine"),
                ( symbol "V", name "Valine"),
                ( symbol "W", name "Tryptophan" ),
                ( symbol "X", name "Undetermined or atypical"),
                ( symbol "Y", name "Tyrosine"),
                ( symbol "Z", name "Glu or Gln" )
*   
*
*****************************************************************************/

/*****************************************************************************
*
*   Genetic Code id's and names from /ncbi/data/gc.prt
*      gc.prt lists the legal start codons and genetic codes fully
*   
		name "Standard" ,
		id 1 ,

		name "Vertebrate Mitochondrial" ,
		id 2 ,

		name "Yeast Mitochondrial" ,
		id 3 ,

		name "Mold Mitochondrial and Mycoplasma" ,
		id 4 ,

		name "Invertebrate Mitochondrial" ,
		id 5 ,

		name "Ciliate Macronuclear and Daycladacean" ,
		id 6 ,

		name "Protozoan Mitochondrial (and Kinetoplast)" ,
		id 7 ,

		name "Plant Mitochondrial" ,
		id 8 ,

		name "Echinoderm Mitochondrial" ,
		id 9 ,

		name "Euplotid Macronuclear" ,
		id 10 ,

		name "Eubacterial" ,
		id 11 ,

*   
*
*****************************************************************************/




