#
# This is a Shiny web application. You can run the application by clicking
# the 'Run App' button above.
#
# Find out more about building applications with Shiny here:
#
#    http://shiny.rstudio.com/
#

library(shiny)
library(tidyverse)
library(magick)

# Define UI for application that draws a histogram
ui <- fluidPage(

    # Application title
    titlePanel("Vectorize Image for Drawing Robot"),

    # Sidebar with a slider input for number of bins 
    sidebarLayout(
        sidebarPanel(
            fileInput(
                "upload",
                "Upload Image To Draw",
                accept = "image/*"
            ),
            sliderInput("radius",
                        "Radius:",
                        min = 0,
                        max = 20,
                        value = 0),
            
            sliderInput("sigma",
                        "Sigma:",
                        min = 0,
                        max = 20,
                        value = 1),
            
            sliderInput("bounds_pct",
                        "Lower and Upper Percent:",
                        min = 0,
                        max = 100,
                        value = c(10,30))
        ),

        # Show a plot of the generated distribution
        mainPanel(
            imageOutput("edge_image",inline = TRUE),
            br(),
            imageOutput("original_image",inline = TRUE)
        )
    )
)

# Define server logic required to draw a histogram
server <- function(input, output) {
    
    toListen <- reactive({
        list(input$radius,input$sigma,input$bounds_pct,input$upload)
    })
    
    # radius = 0
    # sigma = 1
    # lower_pct = 10
    # upper_pct = 30
    # 
    # geom_string <- paste(
    #     radius,
    #     "x",
    #     sigma,
    #     "+",
    #     lower_pct,
    #     "%+",
    #     upper_pct,
    #     ""
    # )
    # 
    # processed_image <- image_read("pixar.jpg") %>%
    #     image_canny(geometry = geom_string) %>%
    #     image_write(tempfile(fileext='jpg'), format = 'jpg')
    
    observeEvent(
        toListen(), {
            
            image_path <- ifelse(
                length(input$upload$datapath),
                as.character(input$upload$datapath),
                "olin.jpg"
            )

            uploaded_image <- image_read(image_path) %>%
            image_write(tempfile(fileext='jpg'), format = 'jpg')
            
            output$original_image <- renderImage({
                
                # filename <-  file.path(
                #     './images', 
                #     paste('image', input$n, '.jpg', sep = '')) %>%
                #     normalizePath()
                filename <-  uploaded_image
                
                # Return a list containing the filename and alt text
                list(
                    src = filename,
                    alt = paste("Original Image"),
                    width = "70%",
                    height = "auto",
                    inline = TRUE
                )
                
                
            }, deleteFile = FALSE)
            
            
            radius = input$radius
            sigma = input$sigma
            lower_pct = input$bounds_pct[1]
            upper_pct = input$bounds_pct[2]
            
            geom_string <- paste(
                radius,
                "x",
                sigma,
                "+",
                lower_pct,
                "%+",
                upper_pct,
                ""
            )
            
            processed_image <- image_read(image_path) %>%
                image_canny(geometry = geom_string) %>%
                image_write(tempfile(fileext='jpg'), format = 'jpg')
            
            output$edge_image <- renderImage({
                
                # filename <-  file.path(
                #     './images', 
                #     paste('image', input$n, '.jpg', sep = '')) %>%
                #     normalizePath()
                filename <-  file.path(
                    processed_image
                ) %>%
                    normalizePath()
                
                # Return a list containing the filename and alt text
                list(
                    src = filename,
                    alt = paste("Original Image"),
                    width = "70%",
                    height = "auto",
                    inline = TRUE
                )
                
                
            }, deleteFile = FALSE)
        }
    )
}

# Run the application 
shinyApp(ui = ui, server = server)
