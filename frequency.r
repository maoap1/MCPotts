rm(list = ls())
n = 1001 #pocet pozorovani

filenames = c("frequency1.txt")
for (i in 2:n) 
{
  filenames = c(filenames,paste("frequency",i,".txt", sep = "", collapse = ""))
}

mylist <- lapply(X = filenames, FUN = read.table, comment.char = '%', row.names = 1)


frequencies = cbind(mylist[[1]][["V2"]], mylist[[2]][["V2"]])
for (i in 3:n) 
{
  frequencies = cbind(frequencies, mylist[[i]][["V2"]])
}

plot(frequencies[1,])
plot(frequencies[2,])
plot(frequencies[3,])
plot(frequencies[4,])

# konverguje prumer?
x = c(mean(mylist[[1]][["V1"]]))
for (i in 2:n) 
{
  x =c(x, mean(mylist[[i]][["V1"]]))  
}
plot(x)
# asi NE

# konverguje median?
x = c(median(mylist[[1]][["V1"]]))
for (i in 2:n) 
{
  x =c(x, median(mylist[[i]][["V1"]]))  
}
plot(x)
#asi NE

#konverguje maximum?
x = c(max(mylist[[1]][["V1"]]))
for (i in 2:n) 
{
  x =c(x, max(mylist[[i]][["V1"]]))  
}
plot(x)
# mozna, vypada to nadejne

#smerodatna odchylka?
x = c(sd(mylist[[1]][["V1"]]))
for (i in 2:n) 
{
  x =c(x, sd(mylist[[i]][["V1"]]))  
}
plot(x)

