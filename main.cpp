#include "Sky.hpp"
int main(int argc, char *argv[])
//int main()
{
    Sky sky;

    //w.setSpaceSize(1440, 900);
    sky.setSpaceSize(4000, 3000);
    sky.setBackgroundType(Sky::BG_REPEATED);
    sky.setBackground("/home/dude/Open/bg.jpg");
    sky.setLiveThumbnailMode(true);
    return sky.Run();
}
