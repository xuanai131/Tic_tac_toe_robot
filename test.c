#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <math.h>
#include <stdlib.h>
#include <softPwm.h>
#include <mysql/mysql.h>

#define s_pwm 13

#define led 7
#define direction1 11
#define pulse1 12
#define direction2 15
#define pulse2 16


int pulse1_cur=0, pulse2_cur=0;
float alpha1_cur=0, alpha2_cur=0;
int p1, p2;
float m;
int d;
float l1 = 0.26;
float l2 = 0.17;
float t1, t2;
float last_t1, last_t2;
float c1, c2, s1, s2;
int chessboard_size = 3;
float square_size = 0.3;
float yd = 0.1;

float f;   //Hz
void pulse_1(float pulse, float omega)  //omega: deg/s 
{
    if (pulse>=0) { d=0; }
    else { d=1; }
    pulse1_cur+=pulse;
    f = 1920*omega/9;
    digitalWrite(direction1, d);
    for (int i=0; i<abs(pulse); i++)
    {
        digitalWrite(pulse1, HIGH);
        delayMicroseconds(500000/f);
        digitalWrite(pulse1, LOW);
        delayMicroseconds(500000/f);
    }
}
void pulse_2(float pulse, float omega)  //omega: deg/s
{
    if (pulse>=0) { d=1; }
    else { d=0; }
    pulse2_cur+=pulse;
    f = 1260*omega/9;
    digitalWrite(direction2, d);
    for (int i=0; i<abs(pulse); i++)
    {
        digitalWrite(pulse2, HIGH);
        delayMicroseconds(500000/f);
        digitalWrite(pulse2, LOW);
        delayMicroseconds(500000/f);
    }
}

void motor_1(float degree, float omega)  //omega: deg/s
{
    alpha1_cur += degree;
    if (degree>=0) { d=0; }
    else { d=1; degree=-degree; }
    m = degree*12800/180;
    f = 1920*omega/9;
    digitalWrite(direction1, d);
    for (int i=0; i<m; i++)
    {
        digitalWrite(pulse1, HIGH);
        delayMicroseconds(500000/f);
        digitalWrite(pulse1, LOW);
        delayMicroseconds(500000/f);
        //pulse1_cur += p1;
    }
}
void motor_2(float degree, float omega)  //omega: deg/s
{
    alpha2_cur += degree;
    if (degree>=0) { d=1; }
    else { d=0; degree=-degree; }
    m = degree*9500/180;
    f = 1260*omega/9;
    digitalWrite(direction2, d);
        for (int i=0; i<m; i++)
        {
            digitalWrite(pulse2, HIGH);
            delayMicroseconds(500000/f);
            digitalWrite(pulse2, LOW);
            delayMicroseconds(500000/f);
            //pulse2_cur += p2;
        }
}

void up()
{
    softPwmWrite(s_pwm, 2.5);
    delay(500);
    softPwmWrite(s_pwm, 0);
}

void down()
{
    softPwmWrite(s_pwm, 7.5);
    delay(500);
    softPwmWrite(s_pwm, 0);
}

void compute_angle(float xP, float yP)
{
    yP = -yP;
    xP = -xP;
    c2= (pow(xP, 2) + pow(yP, 2) - pow(l2, 2) - pow(l1, 2)) / (2*l1*l2);
    float temp;
    if (1-pow(c2, 2) >= 0) { temp = 1-pow(c2, 2); }
    else { temp = 1-pow(c2, 2); }
    s2 = sqrt(temp);
    t2 = atan2(s2, c2);

    c1=(-l2*c2-l1)*yP + xP*l2*s2;
    s1=(l2*c2+l1)*xP + yP*l2*s2;
    t1 = atan2(s1, c1);
    t1 = t1*180/M_PI+90;
    t2 = t2*180/M_PI-90;
    last_t1 = t1; last_t2 = t2;
    //if (isnan(t1)) { t1 = last_t1; }
    //if (isnan(t2)) { t1 = last_t2; }
    //printf("t1: %f, t2: %f\n", t1*180/M_PI, t2*180/M_PI);
}


void rotate(float alpha1, float alpha2, float w)
{
    float a1, a2;
    int ll;
    if (fabs(alpha1) > fabs(alpha2)) { a1 = 0.5; ll = alpha1 * 2; a2 = a1 * alpha2/alpha1; }
    else if (fabs(alpha1) < fabs(alpha2)) { a2 = 0.5; ll = alpha2 * 2; a1 = a2 * alpha1/alpha2; }
    else if (fabs(alpha1) == fabs(alpha2)) { a1 = 0.5; a2 = 0.5; ll = alpha1 * 2; }
    if (alpha1>=0) { a1 = fabs(a1); }
    else { a1 = -fabs(a1); }
    if (alpha2>=0) { a2 = fabs(a2); }
    else { a2 = -fabs(a2); }
    ll = abs(ll);
    for (int i=0; i<ll; i++)
    {
        motor_1(a1, w/2);
        motor_2(a2, w/2);
    }
    
    // motor_1(alpha1 - ll*a1, w/2);
    // motor_2(alpha2 - ll*a2, w/2);
}


void move(float X, float Y)
{
    compute_angle(X, Y);
    // printf("t1: %f, t2; %f\n", t1, t2);
    rotate(t1, t2, 20);
    delay(2000);
    rotate(-alpha1_cur, -alpha2_cur, 20);   //home
    //printf("alpha2_cur: %f, alpha2_cur; %f\n", alpha1_cur, alpha2_cur);
}

void draw_line(float cxi, float cyi, float cxf, float cif)
{

}

void draw_x(float chessboard_x, float chessboard_y)
{
    //chessboard_x = chessboard_size - chessboard_x;
    float x = (chessboard_x - 0.5) * square_size / chessboard_size - 0.15;
    float y = (chessboard_y - 0.5) * square_size / chessboard_size + yd;
    compute_angle(x - square_size/(chessboard_size*3), y + square_size/(chessboard_size*3));
    rotate(t1-alpha1_cur, t2-alpha2_cur, 20);
    down(); // ha but xuong
    // printf("t1: %f, t2; %f\n", t1, t2);
    //printf("alpha1_cur: %f, alpha2_cur; %f\n", alpha1_cur, alpha2_cur);
    for (int cor=1; cor<51; cor++)
    {
        compute_angle(x + - square_size/(chessboard_size*3) + 2*square_size/(chessboard_size*3) * cor/50, y + square_size/(chessboard_size*3) - 2*square_size/(chessboard_size*3) * cor/50);
        // printf("alpha2_cur: %f, alpha2_cur; %f\n", alpha1_cur, alpha2_cur);
        rotate(t1-alpha1_cur, t2-alpha2_cur, 20);
    }
    up(); //nhac but len
    compute_angle(x + square_size/(chessboard_size*3), y + square_size/(chessboard_size*3));
    rotate(t1-alpha1_cur, t2-alpha2_cur, 20);
    down(); // ha but xuong
    for (int cor=1; cor<51; cor++)
    {
        compute_angle(x + square_size/(chessboard_size*3) - 2*square_size/(chessboard_size*3) * cor/50, y + square_size/(chessboard_size*3) - 2*square_size/(chessboard_size*3) * cor/50);
        rotate(t1-alpha1_cur, t2-alpha2_cur, 20);
    }
    up(); //nhac but len
    
    rotate(-alpha1_cur, -alpha2_cur, 20);
    //printf("alpha1_cur: %f, alpha2_cur; %f/////////////////////////////////////////////////\n", alpha1_cur, alpha2_cur);
}

void draw_o(float chessboard_x, float chessboard_y)
{
    //chessboard_x = chessboard_size - chessboard_x;
    float x = (chessboard_x - 0.5) * square_size / chessboard_size - 0.15;
    float y = (chessboard_y - 0.5) * square_size / chessboard_size + yd;
    float r = square_size/(chessboard_size*3);
    compute_angle(x, y + r);
    rotate(t1-alpha1_cur, t2-alpha2_cur, 20);
    down(); // ha but xuong
    for (int i=0; i<100; i++)
    {
        float x_o = x + r*sin(2*M_PI*i/100);
        float y_o = y + r*cos(2*M_PI*i/100);
        compute_angle(x_o, y_o);
        // printf("t1: %f, t2; %f\n", t1, t2);
        rotate(t1-alpha1_cur, t2-alpha2_cur, 20);
    }
    up(); //nhac but len
    rotate(-alpha1_cur, -alpha2_cur, 30);
    //printf("alpha1_cur: %f, alpha2_cur; %f/////////////////////////////////////////////////\n", alpha1_cur, alpha2_cur);
}



int main()
{
    delay(30);
    printf("Caro Control");
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost";
    printf("///////////////////");
    char *user = "admin_rasp2";
    char *password = "1";
    char *database = "admin";
    
    wiringPiSetupPhys();
    softPwmCreate(s_pwm, 0, 50);
    pinMode(direction1, OUTPUT);
    pinMode(pulse1, OUTPUT);
    pinMode(direction2, OUTPUT);
    pinMode(pulse2, OUTPUT);
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);

    
    while(1)
    {
        // ket noi database
        conn = mysql_init(NULL);
        mysql_real_connect(conn,server,user,password,database,0,NULL,0);
        // kiem tra cot isUpdated
        char sql[200];
        sprintf(sql, "select * from caro_control");
        mysql_query(conn,sql);
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res); //row[0]-> red; row[1]->green
        int pulse1_db = atoi(row[0]);
        int pulse2_db = atoi(row[1]);
        int row_col_db = atoi(row[2]);
        int x_o_db = atoi(row[3]);
        int size_db = atoi(row[4]);
        int state_db = atoi(row[5]);
        
        if (state_db==1)
        {
            chessboard_size = size_db;
            int nrow = row_col_db/10;
            int ncol = row_col_db%10;
            if (nrow>chessboard_size | ncol>chessboard_size)
            {
                continue;
            }
            if (x_o_db==0)
            {
                digitalWrite(led, HIGH);
                draw_x(nrow, ncol);
                digitalWrite(led, LOW);
            }
            else
            {
                digitalWrite(led, HIGH);
                draw_o(nrow, ncol);
                digitalWrite(led, LOW);
            }
            
            sprintf(sql, "update caro_control set state=0;");
            mysql_query(conn,sql);
        }
        else if (state_db==2)
        {
            digitalWrite(led, HIGH);
            pulse_1(atoi(row[0]), 40);
            pulse_2(atoi(row[1]), 40);
            sprintf(sql, "update caro_control set state=0;");
            mysql_query(conn,sql);
            digitalWrite(led, LOW);
            continue;
        }
        mysql_close(conn);
    }    
    return 0;
}
