#include "./Motor.h"

Motor stepper1(6, 7, A0, true, 1270);
Motor stepper2(4, 5, A1, false, 1230);
Motor stepper3(3, 2, A2, false, 0);
Motor stepper4(8, 9, A2, false, 0);

void setup()
{
    Serial.begin(115200);

    stepper1.init();
    stepper2.init();
}

void loop()
{
    stepper1.loop();
    stepper2.loop();

    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n');
        int pos = command.substring(0, 1).toInt();
        bool clockwise = !command.substring(1).toInt();

        stepper1.setTargetPos(pos * 540, 0, clockwise);
        stepper2.setTargetPos(pos * 540, 0, clockwise);
    }
}

// void showZero()
// {
//     clock_1.setHands(bottom, right);
//     clock_2.setHands(bottom, left);
//     clock_3.setHands(top, bottom);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(top, right);
//     clock_6.setHands(top, left);
// }

// void showOne()
// {
//     clock_1.setHands(out, out);
//     clock_2.setHands(bottom, bottom);
//     clock_3.setHands(out, out);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(out, out);
//     clock_6.setHands(top, top);
// }
// void showTwo()
// {
//     clock_1.setHands(right, right);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(bottom, right);
//     clock_4.setHands(top, left);
//     clock_5.setHands(top, right);
//     clock_6.setHands(left, left);
// }

// void showThree()
// {
//     clock_1.setHands(right, right);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(right, right);
//     clock_4.setHands(top, left);
//     clock_5.setHands(right, right);
//     clock_6.setHands(top, left);
// }

// void showFour()
// {
//     clock_1.setHands(bottom, bottom);
//     clock_2.setHands(bottom, bottom);
//     clock_3.setHands(top, right);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(out, out);
//     clock_6.setHands(top, top);
// }

// void showFive()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, left);
//     clock_3.setHands(top, right);
//     clock_4.setHands(left, bottom);
//     clock_5.setHands(right, right);
//     clock_6.setHands(top, left);
// }

// void showSix()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, left);
//     clock_3.setHands(top, bottom);
//     clock_4.setHands(bottom, left);
//     clock_5.setHands(right, top);
//     clock_6.setHands(top, left);
// }

// void showSeven()
// {
//     clock_1.setHands(right, right);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(out, out);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(out, out);
//     clock_6.setHands(top, top);
// }

// void showEight()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(right, bottom);
//     clock_4.setHands(left, bottom);
//     clock_5.setHands(right, top);
//     clock_6.setHands(top, left);
// }

// void showNine()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(top, right);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(right, right);
//     clock_6.setHands(top, left);
// }

// void showNull()
// {
//     clock_1.setHands(bottom, bottom);
//     clock_2.setHands(bottom, bottom);
//     clock_3.setHands(bottom, bottom);
//     clock_4.setHands(bottom, bottom);
//     clock_5.setHands(bottom, bottom);
//     clock_6.setHands(bottom, bottom);
// }
