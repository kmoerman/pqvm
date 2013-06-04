# OUTPUT KRONECKER
pdf("kronecker.pdf")
  file_pll = "kronecker-mcp.data"
  file_seq = "kronecker-seq.data"
  data_pll = read.table(file_pll, sep="", skip=2)
  data_seq = read.table(file_seq, sep="", skip=2)
  T1 = data_seq$V1[2]
  data_pll$V2 = T1/data_pll$V2
  boxplot(V2~V1, data=data_pll, main=file_pll, xlab="p", ylab="speedup")
dev.off()

# OUTPUT SIGMA-X
pdf("sigma-x.pdf")

    file_pll1 = "sigma-x-mcp.data"
    file_pll2 = "sigma-x-blk.data"
    file_seq = "sigma-x-seq.data"
    data_pll1 = read.table(file_pll1, sep="", skip=2)
    data_pll2 = read.table(file_pll2, sep="", skip=2)
    data_seq = read.table(file_seq, sep="", skip=2)
    T1 = data_seq$V1[2]
    data_pll1$V2 = T1/data_pll1$V2
    data_pll2$V2 = T1/data_pll2$V2
    boxplot(V2~V1, data=data_pll1, main=file_pll1, xlab="p", ylab="speedup")
    boxplot(V2~V1, data=data_pll2, main=file_pll2, xlab="p", ylab="speedup")
dev.off()

# OUTPUT SIGMA-X
pdf("sigma-z.pdf")

    file_pll1 = "sigma-z-mcp.data"
    file_pll2 = "sigma-z-blk.data"
    file_seq = "sigma-z-seq.data"
    data_pll1 = read.table(file_pll1, sep="", skip=2)
    data_pll2 = read.table(file_pll2, sep="", skip=2)
    data_seq = read.table(file_seq, sep="", skip=2)
    T1 = data_seq$V1[2]
    data_pll1$V2 = T1/data_pll1$V2
    data_pll2$V2 = T1/data_pll2$V2
    boxplot(V2~V1, data=data_pll1, main=file_pll1, xlab="p", ylab="speedup")
    boxplot(V2~V1, data=data_pll2, main=file_pll2, xlab="p", ylab="speedup")
dev.off()

# OUTPUT CONTROL-Z
pdf("control-z.pdf")

    file_pll1 = "control-z-mcp.data"
    file_pll2 = "control-z-blk.data"
    file_seq = "control-z-seq.data"
    data_pll1 = read.table(file_pll1, sep="", skip=2)
    data_pll2 = read.table(file_pll2, sep="", skip=2)
    data_seq = read.table(file_seq, sep="", skip=2)
    T1 = data_seq$V1[2]
    data_pll1$V2 = T1/data_pll1$V2
    data_pll2$V2 = T1/data_pll2$V2
    boxplot(V2~V1, data=data_pll1, main=file_pll1, xlab="p", ylab="speedup")
    boxplot(V2~V1, data=data_pll2, main=file_pll2, xlab="p", ylab="speedup")
dev.off()