//return "<?xml version=\"1.0\" encoding=\"ascii\"?>\n <screen id=\"bl\\"></screen>";

function getScreens() {
        return     "<screen id=\"" + params["id"] +"\">" +
      "<gridLayout height=\"1\" width=\"1\">" +
        "<button x=\"1\" y=\"1\" id=\"button1\">" +
         "<text>plugin screen</text>" +
        "</button>" +
      "</gridLayout>" +
      "<buttongroup id=\"buttongroup\"> <button id=\"button1\"/> </buttongroup>" +
     "<default_answer_element>buttongroup</default_answer_element>" +
    "</screen>";
}
