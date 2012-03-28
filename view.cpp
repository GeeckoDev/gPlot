#include "view.h"
#include <iostream>
#include <cmath>

View::View(FunctionManager* manager) :
    manager(manager),
    x(0.f), xs(0.f), xst(xs),
    y(0.f), ys(0.f), yst(ys),
            zs(1.f), zst(1.f),
    w(BASE_W), h(BASE_H)
{
    manager->update(screenToViewX(0.f),
                    screenToViewX(G2D_SCR_W),
                    G2D_SCR_W);
}

FTYPE View::screenToViewX(float x)
{
    return ( x * this->w / G2D_SCR_W + this->x - this->w/2.f);
}

FTYPE View::screenToViewY(float y)
{
    return (-y * this->h / G2D_SCR_H + this->y + this->h/2.f);
}

float View::viewToScreenX(FTYPE x)
{
    return (( x - this->x + this->w/2.f) * G2D_SCR_W / this->w);
}

float View::viewToScreenY(FTYPE y)
{
    return ((-y + this->y + this->h/2.f) * G2D_SCR_H / this->h);
}

void View::camera()
{
    xs += (xst * MOVE_SPEED * w / BASE_W - xs) * MOVE_ACCEL;
    ys += (yst * MOVE_SPEED * h / BASE_H - ys) * MOVE_ACCEL;
    zs += (pow(2.f, -zst * ZOOM_SPEED) - zs) * ZOOM_ACCEL;
    
    x += xs;
    y += ys;
    w *= zs;
    h *= zs;
    
    if (fabs(xs) > TINY_SPEED || fabs(zs - 1.f) > TINY_ZOOM)
    {
        manager->update(screenToViewX(0.f),
                        screenToViewX(G2D_SCR_W),
                        G2D_SCR_W / 2);
    }
}

void View::drawOrigin()
{
    static float ox, oy;
    static int ivx, ivy;
    static int isx, isy;
    
    ox = viewToScreenX(0.);
    oy = viewToScreenY(0.);
    
    g2dBeginLines((g2dLine_Mode)0);
    {
        g2dSetColor(LITEGRAY);
        
        if (ox >= 0.f && ox < G2D_SCR_W)
        {
            g2dSetCoordXY(ox, 0.f);
            g2dAdd();
            g2dSetCoordXY(ox, G2D_SCR_H);
            g2dAdd();
        }
        if (oy >= 0.f && oy < G2D_SCR_H)
        {
            g2dSetCoordXY(0.f, oy);
            g2dAdd();
            g2dSetCoordXY(G2D_SCR_W, oy);
            g2dAdd();
        }
    }
    g2dEnd();
    
    ivx = (int)screenToViewX(0.f) - 1;
    ivy = (int)screenToViewY(0.f) + 1;
    
    g2dBeginPoints();
    {
        g2dSetColor(DARKGRAY);
        
        while ((isx = viewToScreenX(++ivx)) < G2D_SCR_W)
        {
            if (ivx == 0.f) continue;
            
            g2dSetCoordXY(isx, oy-1.f);
            g2dAdd();
        }
        while ((isy = viewToScreenY(--ivy)) < G2D_SCR_H)
        {
            if (ivy == 0.f) continue;
        
            g2dSetCoordXY(ox+1.f, isy);
            g2dAdd();
        }
    }
    g2dEnd();
}

void View::drawFunction()
{
    static FTYPE a, b;
    static float x, y;
    static Function *f;
    static std::vector<FTYPE> *values;

    for (unsigned int i=0; i<manager->size(); i++)
    {
        f = manager->getFunction(i);
        values = f->getValues();
        if (!f->isValid()) continue;
        
        g2dBeginLines(G2D_STRIP);
        {
            g2dSetColor(BLACK);

            try
            {
                for (unsigned int j=0; j<values->size(); j++)
                {      
                    a = f->getA();
                    b = f->getB();
                    x = viewToScreenX(a + (b-a) * j / values->size());
                    y = viewToScreenY(values->at(j));

                    if (std::isnan(y)) continue;

                    g2dSetCoordXY(x, y);
                    g2dAdd();
                }
            }
            catch (std::out_of_range& e)
            {
                std::cout << "FIXME: out of range in View::drawFunction"
                          << std::endl;
            }
        }
        g2dEnd();
    }
}

void View::draw()
{
    camera();
    drawOrigin();
    drawFunction();
}

void View::controls(Controls* ctrl)
{
    xst = ctrl->buttonPressed(PSP_CTRL_RIGHT) -
          ctrl->buttonPressed(PSP_CTRL_LEFT);
    yst = ctrl->buttonPressed(PSP_CTRL_UP) -
          ctrl->buttonPressed(PSP_CTRL_DOWN);
    zst = ctrl->buttonPressed(PSP_CTRL_RTRIGGER) -
          ctrl->buttonPressed(PSP_CTRL_LTRIGGER);
          
    if (ctrl->buttonPressed(PSP_CTRL_CROSS))
    {
        manager->setFunction(0, "0.5*x^3");
    }
    if (ctrl->buttonPressed(PSP_CTRL_SQUARE))
    {
        manager->setFunction(1, "cos(x)");
    }
    if (ctrl->buttonPressed(PSP_CTRL_TRIANGLE))
    {
        manager->setFunction(2, "x*sin(x)");
    }
    if (ctrl->buttonPressed(PSP_CTRL_CIRCLE))
    {
        manager->setFunction(3, "exp(x)");
    }
}
