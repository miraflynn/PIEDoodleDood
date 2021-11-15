#
# This is a Shiny web application. You can run the application by clicking
# the 'Run App' button above.
#
# Find out more about building applications with Shiny here:
#
#    http://shiny.rstudio.com/
#

library(shiny)
library(shinyBS)
library(tidyverse)
library(magick)

source("generate_fns.R")


radiusTooltip <- "Radius of the Gaussian noise filter. Has minimal effect on the image"
sigmaTooltip <- "Sigma value of the Gaussian noise filter. Increasing sigma makes the lines rounder"
boundsTooltip <- "Threshold for weak and strong edges. Increasing either value will eliminate weaker edges"


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
            
            sliderInput("resolution",
                        "Resolution:",
                        min = 100,
                        max = 1000,
                        value = 500,
                        step = 25
                        ),
            
            sliderInput("radius",
                        "Radius:",
                        min = 0,
                        max = 20,
                        value = 0),
            bsTooltip("radius", radiusTooltip),
            
            sliderInput("sigma",
                        "Sigma:",
                        min = 0,
                        max = 20,
                        value = 1),
            bsTooltip("sigma", sigmaTooltip),
            
            sliderInput("bounds_pct",
                        "Lower and Upper Percent:",
                        min = 0,
                        max = 100,
                        value = c(10,30)),
            bsTooltip("bounds_pct",boundsTooltip),
            
            downloadButton("downloadInstructions",
                           "Generate Instructions")
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
        list(
            input$radius,
            input$sigma,
            input$bounds_pct,
            input$upload,
            input$resolution
            )
    })
    
    observeEvent(
        toListen(), {
            
            image_path <- ifelse(
                length(input$upload$datapath),
                as.character(input$upload$datapath),
                "olin.jpg"
            )

            uploaded_image <- image_read(image_path) %>%
                image_scale(input$resolution) %>%
                image_write(tempfile(fileext='png'), format = 'png')
            
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
            lower_pct = ifelse(lower_pct == 0, 1, lower_pct)
            upper_pct = ifelse(upper_pct == 0, 1, upper_pct)
            
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
                image_scale(input$resolution) %>%
                image_canny(geometry = geom_string) %>%
                # image_quantize(max = 2, colorspace = "gray", dither = FALSE) %>%
                image_write(tempfile(fileext='png'), format = 'png')
            
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
            
            # output$downloadInstructions <- downloadHandler(
            #     filename = "robot_instructions.txt",
            #     content = function(file){
            #         image_read(image_path) %>%
            #             image_scale(input$resolution) %>%
            #             image_canny(geometry = geom_string) %>%
            #             image_write(tempfile(fileext='png'), format = 'png') %>%
            #             readPNG() %>%
            #             as.data.frame() %>%
            #             generate_edges() %>%
            #             generate_instructions() %>%
            #             write(file)
            #     }
            # )
            output$downloadInstructions <- downloadHandler(
                filename = "robot_instructions.txt",
                content = function(file){
                    withProgress(
                        message = "Generating robot path...",
                        detail = "",
                        {
                            incProgress(0.1, detail = "Performing edge detection")
                            x <- image_read(image_path)
                            x <- x %>% image_scale(input$resolution)
                            x <- x %>% image_canny(geometry = geom_string)
                            incProgress(0.1, detail = "Generating image matrix")
                            x <- x %>% image_write(tempfile(fileext='png'), format = 'png')
                            x <- x %>% readPNG()
                            x <- x %>% as.data.frame()
                            incProgress(0.2, detail = "Generating edge list")
                            x <- x %>% generate_edges()
                            incProgress(0.3, detail = "Generating instructions")
                            x <- x %>% generate_instructions()
                            incProgress(0.3, detail = "Ready for download!")
                            x %>% write(file)
                        }
                    )
                }
            )
        }
    )
}

# Run the application 
shinyApp(ui = ui, server = server)
