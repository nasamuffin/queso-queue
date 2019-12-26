#include "chat.h"
#include "obs_text_source.h"
#include "quesoqueue.h"
#include "timer.h"
#include "twitch.h"

int main(int, char **) {
    Twitch t;
    QuesoQueue qq(t);
    qq.LoadLastState();
    Timer ti;
    ObsTextSource textSource("textsource.txt");
    Chat c(qq, ti, textSource);
    c.Connect();
    c.Listen();
}
