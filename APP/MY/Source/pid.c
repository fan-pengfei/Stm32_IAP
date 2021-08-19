#include "pid.h"
#include "timer.h"
PID pid_left_speed,pid_right_speed;
PID pid_left_position,pid_right_position;
first_order_filter_type_t chassis_set_filter[3];
PID serveo[3];
void abs_limit(float *a,float ABS_MAX)
{
	if(*a>ABS_MAX)
		*a=ABS_MAX;
	if(*a<-ABS_MAX)
		*a=-ABS_MAX;
}

void PID_Init(PID *pid,float kp,float ki,float kd)
{
	pid->Kp=kp;
	pid->Ki=ki;
	pid->Kd=kd;
	
	pid->Sv = 0.0;
	pid->Pv = 0.0;
	pid->pout = 0.0;
	pid->iout = 0.0;
	pid->dout = 0.0;
	pid->err=0.0;
	pid->err_last=0.0;
	
	pid->OUT=0.0;
}

void PID_Calc(PID *pid, float Sv_t,float Pv_t,float iout_limit,float pidout_limit,float err_limit)
{
	pid->Sv=Sv_t;
	pid->Pv=Pv_t;
	pid->err=pid->Sv-pid->Pv;
	abs_limit(&pid->err,err_limit);
	pid->total_err+=pid->err;
	pid->pout=pid->Kp*pid->err;
	pid->iout+=pid->Ki*pid->err;
	pid->dout=pid->Kd*(pid->err-pid->err_last);
	abs_limit(&pid->iout,iout_limit);
	pid->OUT=pid->pout+pid->iout+pid->dout;
	pid->err_last=pid->err;
	abs_limit(&pid->OUT,pidout_limit);
}
void set_pwm(int left,int right)
{
	if(left>0)
	{
		TIM_SetCompare1(TIM2,0);
		TIM_SetCompare2(TIM2,left);
	}
	else
	{
		TIM_SetCompare1(TIM2,-left);
		TIM_SetCompare2(TIM2,0);
	}
	if(right>0)
	{
		TIM_SetCompare3(TIM2,0);
		TIM_SetCompare4(TIM2,right);
	}
	else
	{
		TIM_SetCompare3(TIM2,-right);
		TIM_SetCompare4(TIM2,0);
	}
}
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period,fp32 num)
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num = num;
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}

void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
		first_order_filter_type->num / (first_order_filter_type->num + first_order_filter_type->frame_period) * first_order_filter_type->out + 
		first_order_filter_type->frame_period / (first_order_filter_type->num + first_order_filter_type->frame_period) * first_order_filter_type->input;
}
