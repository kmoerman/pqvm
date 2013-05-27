# OUTPUT KRONECKER
pdf("kronecker.pdf")
for (i in 10:12) {
  p = 2^i
  file_pll = paste0("data/kronecker-",p,"-tbb.data")
  file_seq = paste0("data/kronecker-",p,"-seq.data")
  data_pll = read.table(file_pll, sep="", skip=2)
  data_seq = read.table(file_seq, sep="", skip=2)
  T1 = data_seq$V1[2]
  data_pll$V2 = T1/data_pll$V2
  boxplot(V2~V1, data=data_pll, main=file_pll, xlab="p", ylab="S(p)")
}
dev.off()

# OUTPUT SIGMA-X
pdf("sigma-x.pdf")
for (i in 10:22) {
  for (j in 1:(i-1)) {
    p = 2^i
    file_pll = paste0("data/sigma-x-",p,"-",j,"-tbb.data")
    file_seq = paste0("data/sigma-x-",p,"-",j,"-seq.data")
    data_pll = read.table(file_pll, sep="", skip=2)
    data_seq = read.table(file_seq, sep="", skip=2)
    T1 = data_seq$V1[2]
    data_pll$V2 = T1/data_pll$V2
    boxplot(V2~V1, data=data_pll, main=file_pll, xlab="p", ylab="S(p)")
} }
dev.off()

# OUTPUT SIGMA-Z
pdf("sigma-z.pdf")
for (i in 10:22) {
  for (j in 1:(i-1)) {
    p = 2^i
  file_pll = paste0("data/sigma-z-",p,"-",j,"-tbb.data")
  file_seq = paste0("data/sigma-z-",p,"-",j,"-seq.data")
  data_pll = read.table(file_pll, sep="", skip=2)
  data_seq = read.table(file_seq, sep="", skip=2)
  T1 = data_seq$V1[2]
  data_pll$V2 = T1/data_pll$V2
  boxplot(V2~V1, data=data_pll, main=file_pll, xlab="p", ylab="S(p)")
}}
dev.off()

# OUTPUT NORMALIZE
pdf("normalize.pdf")
for (i in 10:22) {
  file_pll = paste0("data/normalize-",p,"-tbb.data")
  file_seq = paste0("data/normalize-",p,"-seq.data")
  data_pll = read.table(file_pll, sep="", skip=2)
  data_seq = read.table(file_seq, sep="", skip=2)
  T1 = data_seq$V1[2]
  data_pll$V2 = T1/data_pll$V2
  boxplot(V2~V1, data=data_pll, main=file_pll, xlab="p", ylab="S(p)")
}
dev.off()