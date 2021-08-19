#ifndef _PID_H
#define _PID_H

typedef float fp32;

typedef __packed struct
{
    fp32 input;        //输入数据
    fp32 out;          //滤波输出的数据
    fp32 num;       //滤波参数
    fp32 frame_period; //滤波的时间间隔 单位 s
}first_order_filter_type_t;
typedef struct
{
	float Sv;
	float Pv;

	float Kp;
	float Ki;
	float Kd;

	float err;
	float err_last;
	float total_err;
	float del_err;
	
	float pout;
	float iout;
	float dout;

	float OUT;
}PID;



void set_pwm(int left,int right);
void abs_limit(float *a,float ABS_MAX);
void PID_Init(PID *pid,float kp,float ki,float kd);
void PID_Calc(PID *pid, float Sv_t,float Pv_t,float iout_limit,float pidout_limit,float err_limit);
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period,fp32 num);

void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input);
float Pid_out_buff(float pid_out);
	
#endif

