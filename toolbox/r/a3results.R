# sudo apt-get install r-cran-xml
library('XML')

a3results <- function(filename) {
  data <- xmlParse(filename)
  return(list(
      trials = xmlToDataFrame(nodes = data["//trial/procedure"], stringsAsFactors = FALSE),
      interactive = data.frame(t(sapply(data["//interactive/entry"], xmlAttrs)))
  ))
}