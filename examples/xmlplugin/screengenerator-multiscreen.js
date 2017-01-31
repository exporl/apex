//return "<?xml version=\"1.0\" encoding=\"ascii\"?>\n <screen id=\"bl\\"></screen>";

function getScreens() {
return     "<screen id=\"" + params["id1"] +"\">" +
      "<gridLayout height=\"1\" width=\"1\">" +
        "<button x=\"1\" y=\"1\" id=\"button1\">" +
         "<text>plugin screen</text>" +
        "</button>" +
      "</gridLayout>" +
      "<buttongroup id=\"buttongroup1\"> <button id=\"button1\"/> </buttongroup>" +
     "<default_answer_element>buttongroup1</default_answer_element>" +
    "</screen>" +
    "<screen id=\"" + params["id2"] +"\">" +
      "<gridLayout height=\"1\" width=\"1\">" +
        "<button x=\"1\" y=\"1\" id=\"button1\">" +
         "<text>plugin screen</text>" +
        "</button>" +
      "</gridLayout>" +
      "<buttongroup id=\"buttongroup1\"> <button id=\"button1\"/> </buttongroup>" +
     "<default_answer_element>buttongroup1</default_answer_element>" +
    "</screen>"+
"<screen id=\"nog\">" +
      "<gridLayout height=\"1\" width=\"1\">" +
        "<button x=\"1\" y=\"1\" id=\"button1\">" +
         "<text>plugin screen</text>" +
        "</button>" +
      "</gridLayout>" +
      "<buttongroup id=\"buttongroup1\"> <button id=\"button1\"/> </buttongroup>" +
     "<default_answer_element>buttongroup1</default_answer_element>" +
    "</screen>";
}
