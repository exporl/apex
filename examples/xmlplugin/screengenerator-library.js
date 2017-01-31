function getScreens() {

return     "<screen id=\"" + params["id"] +"\">" +
      "<gridLayout height=\"1\" width=\"1\">" +
        "<button x=\"1\" y=\"1\" id=\"button1\">" +
         "<text>plugin screen from library</text>" +
        "</button>" +
      "</gridLayout>" +
     "<default_answer_element>buttongroup1</default_answer_element>" +
    "</screen>";

}