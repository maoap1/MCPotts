rm(list = ls())
n = 101 #pocet pozorovani

filenames = c("data1.txt")
for (i in 2:n) 
{
  filenames = c(filenames,paste("data",i,".txt", sep = "", collapse = ""))
}

mylist <- lapply(X = filenames, FUN = read.table, comment.char = '%')


#summary(mylist)
#summary(mylist[[1]][["V1"]]) #takto se pristupuje k prvnimu listu

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

