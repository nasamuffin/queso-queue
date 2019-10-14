#include "chat.h"
#include "quesoqueue.h"
#include "timer.h"
#include "twitch.h"

int main(int argc, char *argv[]) {
    Twitch t;
    QuesoQueue qq(t);
    Timer ti;
    Chat c(qq, ti);
    c.Connect();
    c.WriteMessage("Hello?");
    c.Listen();
}
