# TriCluster 3D gene expression clustering algorithm

Tricluster is the first tri-clustering algorithm for microarray expression clustering. It builds upon our microCluster bi-clustering approach. Tricluster first mines all the bi-clusters across the gene-sample slices, and then it extends these into tri-clusters across time or space (depending on the third dimension). It can find both scaling and shifting patterns.


Lizhuang Zhao and Mohammed J. Zaki. TriCluster: an effective algorithm for mining coherent clusters in 3d microarray data. In ACM SIGMOD Conference on Management of Data. June 2005.

# How to
I. type make

II. Usage: ./triCluster -fFile -s[Times,Samples,Genes] [other options]

        ==================== OPTIONS =====================
        File Name:           -fString
        Minimum Size:        -s[Integer,Interge,Integer]   /*T, S, G*/
        Range Window Size:   -wFloat   /*0.01 by default*/
        Deletion Threshold:  -dFloat   /*1.00 by default*/
        Merging  Threshold:  -mFloat   /*1.00 by default*/
        Unrelated Numbers:   -uFloat   /*mciroCluster will not consider the values refered by this option*/
        delta-T:             -etFloat  /*when trying to get close Time values only*/
        delta-S:             -esFloat  /*when trying to get close Sample values only*/
        delta-G:             -egFloat  /*when trying to get close Gene values only*/
        Record to file:      -rString  /*Output the result to a file */
        output in brief:     -b        /*Output the current status in brief*/
        Output Clusters:     -o123     /*1:Original clusters,  2:Clusters  after deletion, 3:Clusters  after merging*/
        Output Names:        -ntsg     /*t:time names, s:Sample names, g:Gene names*/

        Input File Format:   please use TXT file "datafile.tab" as an example, which is tab-delimited.
        A Running example:   $./r

