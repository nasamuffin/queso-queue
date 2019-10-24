#include "chat.h"
#include "quesoqueue.h"
#include "timer.h"
#include "twitch.h"

int main(int argc, char *argv[]) {
    Twitch t;
    QuesoQueue qq(t);
    qq.LoadLastState();
    Timer ti;
    Chat c(qq, ti);
    c.Connect();
    c.Listen();
}
