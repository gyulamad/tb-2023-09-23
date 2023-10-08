#include "../../../../src/includes/madlib/Tools.hpp"
#include "../../../../src/includes/madlib/graph/graph.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;

GFX gfx;
GUI gui(&gfx, 800, 600);
Button closeOkBtn(&gfx, 10, 10, 100, 30, "Ok");
Button button(&gfx, 10, 10, 80, 30, "Hello");
Button button2(&gfx, 110, 10, 80, 30, "Click!");
Frame frame2(&gfx, 320, 100, 200, 200);
Button lbtn(&gfx, 50, 50, 100, 30, "Hello");

void doit(void*, unsigned int, int, int) {
    button2.setBackgroundColor(red);
    button2.setTextColor(white);
    button2.draw();
}

void draw(void*) {
    frame2.color(green);
    frame2.rect(10, 10, 50, 50);
    frame2.rect(120, 100, 150, 150);
    frame2.fillRect(220, 220, 300, 300);
    frame2.color(red);
    frame2.line(10, 10, 50, 50);
    frame2.line(120, 100, 150, 150);
    frame2.line(220, 220, 300, 300);
    frame2.color(blue);
    frame2.line(50, 10, 10, 50);
    frame2.line(300, 220, 220, 300);
    frame2.rect(25, 25, 325, 325);
    frame2.color(black);
    frame2.line(325, 25, 25, 325);
    frame2.color(orange);
    frame2.hLine(30, 30, 130);
    frame2.color(yellow);
    frame2.vLine(30, 30, 130);
    // TODO: bach painting...
}

void close(void*, unsigned int, int, int) {
    gfx.close = true;
}

void lbtn_draw(void* context) {
    Area* area = (Area*)context;
    area->color(red);
    area->fillRect(5, 5, 25, 25);
}

int graph_manual_test1()
{
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.onTouchHandlers.push_back(close);
    gui.child(&closeOkBtn);

    Frame frame(&gfx, 100, 100, 200, 200);
    // frame.fixed = true;    
    button2.onTouchHandlers.push_back(doit);
    Button button3(&gfx, 210, 10, 80, 30, "Sticky");
    button3.sticky = true;
    Button button4(&gfx, 310, 10, 80, 30, "Hello4");
    Button button5(&gfx, 10, 50, 250, 50, "Big Willy");
    Button button6(&gfx, 210, 210, 80, 30, "Close");
    button6.onTouchHandlers.push_back(close);
    Label label1(&gfx, 100, 130, 100, 20, "Label1");
    frame.child(&button);
    frame.child(&button2);
    frame.child(&button3);
    frame.child(&button4);
    frame.child(&button5);
    frame.child(&button6);
    frame.child(&label1);
    gui.child(&frame);

    frame2.child(&lbtn);
    frame2.setScrollXYMax(350, 350);
    frame2.onDrawHandlers.push_back(draw);
    gui.child(&frame2);

    lbtn.onDrawHandlers.push_back(lbtn_draw);

    gui.loop();

    return 0;
}
