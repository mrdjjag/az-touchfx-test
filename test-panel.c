/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#undef assert
#define assert(x) do {if(!(x)){fprintf(stderr,"CHECK failed at line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
#include <string.h>
#include <time.h>
static long test_time=1000;
static int test_clock(int unused,struct timespec *ts) {(void)unused;ts->tv_sec=test_time/1000;ts->tv_nsec=(test_time%1000)*1000000;return 0;}
#define CLOCK_MONOTONIC 1
#define clock_gettime test_clock
#include "panel-test.c"
#undef clock_gettime
static int values[16],nrows,surfaces,shown[2],paints,dimmed;
static az_deck_info fixtures[4];
static int cfg_get(const char *id,const char *key,int def) {(void)id;(void)key;return def;}
static void cfg_set(const char *id,const char *key,int value) {(void)id;(void)key;(void)value;}
static int regrow(const char *id,const az_row_v1 *r) {(void)id;assert(nrows<16);int n=nrows++;values[n]=(r->flags&AZ_ROW_DEFAULT_ON)?1:r->default_choice;return n;}
static int val(int h) {assert(h>=0&&h<nrows);return values[h];}
static int info(int d,az_deck_info *o) {assert(d>=0&&d<4);*o=fixtures[d];return 0;}
static az_surface *create(const char *id,const az_surface_v1 *s) {(void)id;assert(s->on_paint);return (az_surface*)(uintptr_t)(++surfaces);}
static void place(az_surface *s,az_rect a) {(void)s;assert(a.w>0&&a.h>0);}
static void show(az_surface *s,int on) {shown[(uintptr_t)s-1]=on;}
static void redraw(az_surface *s) {(void)s;paints++;}
static int regfilter(const char *id,unsigned mask,az_frame_fn f,void *u) {(void)id;(void)u;assert(mask&(1u<<AZ_BUS_MIXER_IN));assert(f==frame);return 0;}
static void fill(az_draw *d,int x,int y,int w,int h,uint32_t c) {(void)d;(void)x;(void)y;(void)w;(void)h;(void)c;}
static void text(az_draw *d,int x,int y,const char *s,int px,uint32_t rgb) {(void)d;(void)x;(void)y;(void)px;if((!strcmp(s,"Vocals")||!strcmp(s,"Instrument"))&&rgb==0x7e858c)dimmed++;}
static az_core_v1 fake={.abi=1,.size=sizeof(az_core_v1),.cfg_get=cfg_get,.cfg_set=cfg_set,.register_row=regrow,.row_value=val,.deck_info=info,.surface_create=create,.surface_place=place,.surface_show=show,.surface_redraw=redraw,.frame_filter=regfilter,.fill_rect=fill,.draw_text=text};
static void controls(unsigned mask,unsigned cross) {
 unsigned char b[100]={0},original[100];
 for(int d=0;d<4;d++) if(mask&(1u<<d)){b[10+d]=255;b[14]|=3u<<(6-2*d);}
 b[15]=(unsigned char)(cross>>2);b[16]=(unsigned char)(cross&3);memcpy(original,b,sizeof b);
 assert(frame(AZ_BUS_MIXER_IN,b,sizeof b,NULL)==AZ_FRAME_OK);assert(!memcmp(original,b,sizeof b));
}
static void step(long delta) {test_time+=delta;tick();}
int main(void) {
 assert(init(NULL)==1);
 assert(!init(&fake));assert(!strcmp(az_mod_v1_entry()->id,ID));assert(selected==-1);assert(values[dim_row]==1);assert(values[follow_row]==0);
 for(int d=0;d<4;d++){fixtures[d].size=sizeof(az_deck_info);fixtures[d].loaded=1;fixtures[d].has_stems=1;fixtures[d].pad_mode=1;}
 step(10);assert(shown[0]&&!shown[1]);
 az_event tap={.kind=AZ_EV_TAP,.x=1100,.y=10};assert(tap_button(&tap,NULL));assert(opened&&shown[1]);
 for(int d=0;d<4;d++){
  tap.x=64+288*d+20;tap.y=430;assert(tap_panel(&tap,NULL));assert(selected==d);
  assert(tap_button(&tap,NULL));assert(!opened);assert(tap_button(&tap,NULL));assert(opened&&selected==d);
 }
 fixtures[0].pad_mode=2;step(100);assert(selected==3&&kind[0]==2);
 fixtures[3].loaded=0;step(100);assert(selected==3);fixtures[3].loaded=1;strcpy(fixtures[3].title,"New track");step(100);assert(selected==3);
 dimmed=0;paint_panel(NULL,(az_rect){48,360,1184,400},NULL);assert(dimmed==2);
 values[dim_row]=0;dimmed=0;paint_panel(NULL,(az_rect){48,360,1184,400},NULL);assert(dimmed==0);
 values[follow_row]=1;controls(1,0);step(100);controls(2,1023);step(100);step(100);assert(selected==3); /* Unknown assignments -> no guess */
 values[assign_row[0]]=1;values[assign_row[1]]=3;values[assign_row[2]]=values[assign_row[3]]=2;
 controls(3,1023);step(100); /* baseline */
 tap_button(&tap,NULL);assert(!opened);
 controls(3,0);step(10);step(79);assert(selected==3);step(1);assert(selected==0&&!opened);
 controls(3,512);step(100);step(100);assert(selected==0); /* ambiguous center */
 controls(3,1023);step(100);step(80);assert(selected==1&&!opened);
 tap_button(&tap,NULL);assert(opened&&selected==1);
 for(int d=0;d<4;d++){
  controls(0,512);step(100);controls(1u<<d,512);step(100);step(80);assert(selected==d);
 }
 values[enable_row]=0;step(100);assert(!shown[0]&&!shown[1]);int old=selected;controls(1,0);step(100);assert(selected==old);
 assert(!tap_button(&tap,NULL));
 puts("PASS: SDK init, unknown deck, per-deck memory, inactive pad, track replacement, default dim toggle, unknown routing, 80ms follow, crossfader, ambiguous mix, all four channels, disabled idle, native frames unchanged.");
 return 0;
}
