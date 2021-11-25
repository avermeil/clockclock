// original array
// int sin_table[12] = {
//     0,
//     1,
//     4,
//     8,
//     11,
//     12,
//     12,
//     11,
//     8,
//     4,
//     1,
//     0};

const byte half_cycle_step_count = 12;
const byte full_cycle_step_count = 24;
const byte pwm_scale = 6;

const byte sin_table[12] = {
    0,
    1,
    2,
    4,
    5,
    6,
    6,
    6,
    5,
    4,
    2,
    1};

const byte randomized_pwm_indexes[] = {0, 11, 1, 10, 2, 9, 3, 8, 4, 7, 5, 6}; // makes PWM less whiny

unsigned long last_step_time = 0;

unsigned int step_time = 1000;

byte pwm_step = 0;
byte alpha_index = 0;
byte beta_index = 4; // offset the second wave

byte actual_table[full_cycle_step_count][pwm_scale] = {};

byte a_power = 0;
byte b_power = 0;
byte c_power = 0;
byte d_power = 0;

byte current_pwm_mask;

byte step_index = 0;

void setup()
{
    Serial.begin(9600);

    /*
        Global variables! love em
    */
    for (byte i = 0; i < 12; i++)
    {
        step_index = i;
        step();
        for (byte j = 0; j < 12; j++)
        {
            pwm_step = j;
            setPwmMask();
            actual_table[i][randomized_pwm_indexes[j]] = current_pwm_mask;
        }
    }
    for (byte i = 0; i < 12; i++)
    {
        step_index = i;
        step();
        for (byte j = 0; j < 12; j++)
        {
            pwm_step = j;
            setPwmMask();
            actual_table[i + 12][randomized_pwm_indexes[j]] = current_pwm_mask;
        }
    }

    // set pins 4,5,6,7 to output
    DDRD = DDRD | B11110000;
}

void loop()
{
    PORTD = actual_table[step_index][pwm_step];

    pwm_step++;
    if (pwm_step == pwm_scale)
    {
        pwm_step = 0;
    }

    unsigned long current_time = micros();

    if (last_step_time < current_time - step_time)
    {

        last_step_time = current_time;

        step_index++;
        if (step_index == full_cycle_step_count)
        {
            step_index = 0;
        }
    }
}

void step()
{

    if (alpha_index == full_cycle_step_count)
    {
        alpha_index = 0;
    }

    if (beta_index == full_cycle_step_count)
    {
        beta_index = 0;
    }

    if (alpha_index < half_cycle_step_count)
    {
        a_power = sin_table[alpha_index];
        b_power = 0;
    }
    else
    {
        a_power = 0;
        b_power = sin_table[alpha_index - half_cycle_step_count];
    }

    if (beta_index < half_cycle_step_count)
    {
        c_power = sin_table[beta_index];
        d_power = 0;
    }
    else
    {
        c_power = 0;
        d_power = sin_table[beta_index - half_cycle_step_count];
    }

    // Serial.print(alpha_index);
    // Serial.print(",");
    // Serial.print(beta_index);
    // Serial.print(",");

    // Serial.print(a_power);
    // Serial.print(",");
    // Serial.print(-b_power);
    // Serial.print(",");

    // Serial.print(c_power);
    // Serial.print(",");

    // Serial.println(-d_power);

    alpha_index++;

    beta_index++;
}

void setPwmMask()
{

    byte mask = 0b0000;

    if (a_power >= pwm_step)
    {
        mask += 0b0001;
    }
    if (b_power >= pwm_step)
    {
        mask += 0b0010;
    }
    if (c_power >= pwm_step)
    {
        mask += 0b0100;
    }
    if (d_power >= pwm_step)
    {
        mask += 0b1000;
    }

    current_pwm_mask = mask << 4;
}
