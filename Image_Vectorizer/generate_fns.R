library(tidyverse)
library(magick)
library(png)

generate_matrix <- function(img){
  img_df <- img %>%
    image_write(tempfile(fileext='png'), format = 'png') %>%
    readPNG() %>%
    as.data.frame()
  return(img_df)
}

generate_edges <- function(img_df){
  
  # Create dataframe of ones
  ones <- data.frame(x = integer(), y = integer())
  for(i in 1:nrow(img_df)){ # For each row and column in the image dataframe
    for(j in 1:ncol(img_df)){
      if(img_df[i,j] == 1){ # if the pixel at this location is 1
        # print(x[i,j])
        new <- data.frame(r = i, c = j) # make a dataframe with this location
        ones <- rbind(ones,new) # bind the new dataframe to the existing one
        # basically add a row to it
      }
    }
  }
  
  # Remove L shapes of pixels
  
  for(i in 1:nrow(ones)){ # For each row and column in the image dataframe
    if( # If this pixel isn't on the edge of the image
      # Technically it should check each neighbor but whatever
      ones[i,1] >= 2 & 
      ones[i,1] <= nrow(img_df) - 1 & 
      ones[i,2] >= 2 & 
      ones[i,2] <= ncol(img_df) - 1
    ){
      r <- ones[i,1] # get this pixel's row and column
      c <- ones[i,2]
      up <- img_df[r-1,c] == 1 #Booleans for if each non-diagonal neighbor is white
      left <- img_df[r,c-1] == 1
      down <- img_df[r+1,c] == 1
      right <- img_df[r,c+1] == 1
      if( # If L-shaped
        (up & left & !down) |
        (up & right & !down) |
        (down & left & !up) |
        (down & right & !up) |
        (up + left + right + down > 2)
      ){
        img_df[r,c] = 0 # then my value needs to be zero
        # ones <- ones %>%
        #   filter(r != r, c != c)
      }
    }
  }
  
  # We have to recalculate ones again. The L-remover would get more complicated
  # if I removed ones from the ones dataframe as it ran
  ones <- data.frame(x = integer(), y = integer())
  for(i in 1:nrow(img_df)){ # For each row and column in the image dataframe
    for(j in 1:ncol(img_df)){
      if(img_df[i,j] == 1){ # if the pixel at this location is 1
        # print(x[i,j])
        new <- data.frame(r = i, c = j) # make a dataframe with this location
        ones <- rbind(ones,new) # bind the new dataframe to the existing one
        # basically add a row to it
      }
    }
  }
  
  # Generate a list of edges
  
  # Create a dataframe of relative neighbor positions. Because everything iterates
  # over rows and columns going from 0 to max, we only need to check neighboring 
  # pixels that are to the right or below the current pixel
  
  neighborrs <- c(0,1,1,1) # make these positions as lists
  neighborcs <- c(1,-1,0,1)
  
  nbrs <- data.frame(
    r = neighborrs,
    c = neighborcs
  ) # make a dataframe from the lists
  
  edges <- data.frame(
    x1 = integer(),
    y1 = integer(),
    x2 = integer(),
    y2 = integer()
  ) # create an empty dataframe for each edge
  
  for(a in 1:nrow(ones)){ # for each pixel of value one
    my_pos <- ones[a,]
    my_posr <- my_pos[1,1] # get the row and column of that pixel
    my_posc <- my_pos[1,2]
    for(b in 1:nrow(nbrs)){ # for each neighbor pixel
      nbr_posr_rel <- nbrs[b,1] # get the relative row and column
      nbr_posc_rel <- nbrs[b,2]
      nbr_posr <- my_posr + nbr_posr_rel #calculate the absolute row and column
      nbr_posc <- my_posc + nbr_posc_rel
      if(
        nbr_posr >= 1 & 
        nbr_posr <= nrow(img_df) & 
        nbr_posc >= 1 & 
        nbr_posc <= ncol(img_df)
      ){ # if the neighbor pixel is within the bounds of the image
        if(img_df[nbr_posr,nbr_posc] == 1){ # if the neighbor pixel is 1
          edge <- data.frame( # create a 1 row dataframe with the pixel value
            x1 = my_posc, 
            y1 = my_posr,
            x2 = nbr_posc,
            y2 = nbr_posr
          )
          edges <- rbind(edges,edge) # add that row to the edges dataframe
        }
      }
    }
  }
  return(edges)
}

generate_instructions <- function(edges){
  # robot instruction generator
  
  posx <- edges[1,1]
  posy <- edges[1,2]
  queue <- edges
  instructions <- c(paste("GO", sprintf("%03d", posx), sprintf("%03d", posy), sep = ",")) # go to the first point
  instructions <- c(instructions, "DN........")
  while(nrow(queue) > 0){ # while there are still edges in the queue
    df <- queue %>% # check for queued edges to draw that connect to my current point
      filter((x1 == posx & y1 == posy) | (x2 == posx & y2 == posy))
    if(nrow(df) == 0){ # If there are no queued edges,then go to the start of the
      # next edge in the queue
      instructions <- c(instructions, "UP........")
      posx <- queue[1,1]
      posy <- queue[1,2]
      instructions <- c(instructions, paste("GO", sprintf("%03d", posx), sprintf("%03d", posy), sep = ","))
      instructions <- c(instructions, "DN........")
    } else { # If there are queued edges
      next_edge <- df[1,]
      if(posx == next_edge[1,1] & posy == next_edge[1,2]){ # Find the next point
        # to travel to
        new_posx <- next_edge[1,3]
        new_posy <- next_edge[1,4]
      } else {
        new_posx <- next_edge[1,1]
        new_posy <- next_edge[1,2]
      }
      #Drive to that point
      instructions <- c(instructions, paste("GO", sprintf("%03d", new_posx), sprintf("%03d", new_posy), sep = ","))
      queue <- queue %>% #Remove the edge we just traveled from the queue
        filter(!((x1 == posx & y1 == posy & x2 == new_posx & y2 == new_posy)|
                   (x2 == posx & y2 == posy & x1 == new_posx & y1 == new_posy)))
      posx <- new_posx # Current position is now the next position
      posy <- new_posy
    }
  }
  return(instructions)
}