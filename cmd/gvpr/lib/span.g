/* derive simple spanning tree */
BEG_G { $tvtype = TV_dfs}
E [!isSubnode($T,$.head) || !isSubnode($T,$.tail)]
