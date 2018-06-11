Customizing appearance
======================

The appearance of virtually every element on the screen can be
customized. On a small scale, every element on screen can be customized
by using tags such as bgcolor or fontsize per element, but on a broader
scale every element can be customized by adding a style element and
specifying a style sheet to be used.

Internally the Qt Stylesheet mechanism is used. For a complete overview
on how this works, we refer to the Qt manual, which can also be found on
the internet at <http://doc.trolltech.com/4.3/stylesheet.html>.

Basically, a style sheet consists of a *selector* and a series of
properties that can be set.

```css
    QPushButton#button1 {
        background-color: red;
    }
```

Here the selector is on line 1 and the property background-color is set
to red.

A selector again consists of two parts: the type of elements that will
be affected and the name of the concrete element to select. Here the
type is QPushButton and the name is button1. The latter name would be
what you specify as an id in the experiment file.

All elements used in APEX are listed [here](#table1). Several
generic names of element are given in [here](#table2).

In APEX, stylesheets can be specified at 3 levels: inside the
`<screens>` element in the `<apex_style>` element: if placed here, every
element on screen will be affected, including the panel buttons and the
main menu. If placed in the `<style>` element, the stylesheet will
affect all screens, but not the panel or menus. If placed in a specific
screen element, only that element will be affected.


Screen elements for use in style sheets.

| Element name | Screen element |   Description     | <a name="table1"/>
|--------------|----------------| ------------------|
| QPushButton  |  button        | clickable button  |
| QLabel       |  label         | text label        |
|              |  picture       | picture           |


| Name        | Type        | Description                  | <a name="table2"/>
|-------------|-------------|------------------------------|
| background  | QWidget     | The background of the screen |
| startbutton | QPushButton | The start button             |

The color names that can be used, are those specified in the css
standard. A list can be found at <http://www.w3schools.com/css/css_colornames.asp>.
