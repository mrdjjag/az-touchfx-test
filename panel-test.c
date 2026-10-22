/* SPDX-License-Identifier: GPL-3.0-only */
/* Local SDK display prototype. Never changes audio or native pad modes. */
#include "azmod-mod.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define ID "mrdjjag-touchfx-test"
static const az_core_v1 *C;
static az_surface *button_surface, *panel_surface;
static int enable_row=-1, dim_row=-1, follow_row=-1, assign_row[4];
static int selected=-1, opened, active, pending=-1, was_follow;
static int kind[4]; /* 0 unknown, 1 STEMS, 2 Drum Roll (visual context only) */
static az_deck_info decks[4];
static unsigned previous_mask, open_mask;
static long pending_at, redraw_at;
static uint64_t mixer_state;
static long now_ms(void) {
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC,&ts)) return 0;
    return (long)(ts.tv_sec*1000L+ts.tv_nsec/1000000L);
}
static uint64_t decode(const unsigned char *b) {
    uint64_t s=UINT64_C(1)<<63;
    for(int d=0;d<4;d++) s|=(uint64_t)(((unsigned)b[10+d]<<2)|((b[14]>>(6-2*d))&3))<<(10*d);
    return s|((uint64_t)(((unsigned)b[15]<<2)|(b[16]&3))<<40);
}
static int frame(int bus,unsigned char *b,int len,void *u) {
    (void)u;
    if(bus==AZ_BUS_MIXER_IN && len>=17)
        __atomic_store_n(&mixer_state,decode(b),__ATOMIC_RELEASE);
    return AZ_FRAME_OK;
}
static int only(unsigned m) {
    if(!m || (m&(m-1))) return -1;
    for(int d=0;d<4;d++) if(m==(1u<<d)) return d;
    return -1;
}
static void follow(long now) {
    uint64_t s=__atomic_load_n(&mixer_state,__ATOMIC_ACQUIRE);
    int configured=1,follow_on=C->row_value(follow_row)!=0;
    int assignments[4];
    for(int d=0;d<4;d++) {
        assignments[d]=C->row_value(assign_row[d]);
        if(assignments[d]<1 || assignments[d]>3) configured=0;
    }
    if(!(s>>63) || !follow_on || !configured) {pending=-1;was_follow=0;return;}
    unsigned mask=0,cross=(unsigned)(s>>40)&1023u;
    for(int d=0;d<4;d++) {
        unsigned v=(unsigned)(s>>(10*d))&1023u;
        if(v>=((open_mask&(1u<<d))?4u:8u)) mask|=1u<<d;
    }
    open_mask=mask;
    for(int d=0;d<4;d++) {
        int a=assignments[d]; /* 1=A, 2=THRU, 3=B */
        if(a!=2 && !((cross>3 || a==1) && (cross<1020 || a==3))) mask&=~(1u<<d);
    }
    if(!was_follow) {was_follow=1;previous_mask=mask;pending=-1;return;}
    if(mask!=previous_mask) {previous_mask=mask;pending=only(mask);pending_at=now;}
    if(pending>=0 && now-pending_at>=80) {
        int d=pending;pending=-1;
        if(decks[d].loaded==1) {selected=d;C->cfg_set(ID,"deck",d);}
    }
}
static void paint_button(az_draw *draw,az_rect a,void *u) {
    (void)u;char s[64];
    C->fill_rect(draw,a.x,a.y,a.w,a.h,0x243035);
    if(selected>=0) snprintf(s,sizeof s,"Panel test  CH %d",selected+1);
    else snprintf(s,sizeof s,"Panel test");
    C->draw_text(draw,a.x+8,a.y+8,s,18,0xffffff);
}
static void paint_panel(az_draw *draw,az_rect a,void *u) {
    (void)u;
    C->fill_rect(draw,a.x,a.y,a.w,a.h,0x182028);
    C->draw_text(draw,a.x+16,a.y+8,"DISPLAY TEST - NO AUDIO EFFECTS",22,0xe0c080);
    int width=(a.w-32)/4;
    for(int d=0;d<4;d++) {
        int x=a.x+16+d*width;char s[48];
        C->fill_rect(draw,x,a.y+55,width-10,52,selected==d?0x356a50:0x303b45);
        snprintf(s,sizeof s,"CH %d%s",d+1,decks[d].loaded==1?"":" (empty)");
        C->draw_text(draw,x+8,a.y+66,s,20,0xffffff);
    }
    if(selected<0) {
        C->draw_text(draw,a.x+16,a.y+130,"Choose a loaded channel above.",20,0xffffff);return;
    }
    int d=selected;char s[192];
    snprintf(s,sizeof s,"CH %d  %s  %s",d+1,kind[d]==1?"STEMS":kind[d]==2?"DRUM ROLL":"Native page",decks[d].playing==1?"PLAYING":"");
    C->draw_text(draw,a.x+16,a.y+128,s,22,0xffffff);
    /* SDK deck_info has no individual stem mute/gain fields. Do not guess them. */
    uint32_t ink=C->row_value(dim_row) && decks[d].stems_on!=1?0x7e858c:0xffffff;
    C->draw_text(draw,a.x+16,a.y+175,"Vocals",24,ink);
    C->draw_text(draw,a.x+240,a.y+175,"Instrument",24,ink);
    snprintf(s,sizeof s,"%s",decks[d].loaded==1?decks[d].title:"Track unloaded - channel memory retained");
    C->draw_text(draw,a.x+16,a.y+225,s,18,0xd0d8dd);
    C->draw_text(draw,a.x+16,a.y+274,"Read-only indicators. No stem mute, sampler or FX controls.",16,0xc0c8cc);
    C->draw_text(draw,a.x+16,a.y+312,"Tap Panel test to close. Fader mapping is configured in Mod Menu.",16,0xc0c8cc);
}
static int tap_button(const az_event *e,void *u) {
    (void)u;
    if(!active || e->kind!=AZ_EV_TAP) return 0;
    opened=!opened;C->surface_show(panel_surface,opened);return 1;
}
static int tap_panel(const az_event *e,void *u) {
    (void)u;
    if(!active || !opened || e->kind!=AZ_EV_TAP) return 0;
    if(e->y>=415 && e->y<467 && e->x>=64 && e->x<1216) {
        int d=(e->x-64)/288;
        if(d>=0 && d<4 && decks[d].loaded==1) {
            selected=d;pending=-1;C->cfg_set(ID,"deck",d);C->surface_redraw(panel_surface);
        }
    }
    return 1;
}
static const az_surface_v1 BUTTON={.size=sizeof(az_surface_v1),.layer=AZ_LAYER_OVER_EP147,.views=AZ_VIEW_BIT(AZ_VIEW_DECK),.on_paint=paint_button,.on_input=tap_button};
static const az_surface_v1 PANEL={.size=sizeof(az_surface_v1),.layer=AZ_LAYER_OVER_EP147,.views=AZ_VIEW_BIT(AZ_VIEW_DECK),.on_paint=paint_panel,.on_input=tap_panel};
static void tick(void) {
    int enabled=C->row_value(enable_row)!=0;
    if(enabled!=active) {
        active=enabled;C->surface_show(button_surface,active);C->surface_show(panel_surface,active&&opened);
        pending=-1;was_follow=0;
    }
    if(!active) return;
    long now=now_ms();
    for(int d=0;d<4;d++) {
        az_deck_info info;memset(&info,0,sizeof info);info.size=sizeof info;info.pad_mode=-1;
        if(C->deck_info(d,&info)) {memset(&decks[d],0,sizeof decks[d]);continue;}
        info.title[sizeof(info.title)-1]=0;decks[d]=info;
        if(info.pad_mode==2) kind[d]=2;
        else if(info.pad_mode==1 && info.has_stems==1) kind[d]=1;
    }
    follow(now);
    if(now-redraw_at>=250) {
        redraw_at=now;C->surface_redraw(button_surface);
        if(opened) C->surface_redraw(panel_surface);
    }
}
static int row(const char *key,const char *label,int type,unsigned flags,const char *help,const char *const *choices,int n) {
    az_row_v1 r;memset(&r,0,sizeof r);r.size=sizeof r;r.key=key;r.label=label;r.kind=type;r.flags=flags;r.help=help;r.choices=choices;r.n_choices=n;
    return C->register_row(ID,&r);
}
static int init(const az_core_v1 *core) {
    if(!core || core->abi!=AZ_MOD_ABI || !AZ_HAS(core,surface_create) || !AZ_HAS(core,deck_info) || !AZ_HAS(core,frame_filter) || !AZ_HAS(core,register_row) || !AZ_HAS(core,surface_redraw)) return 1;
    C=core;
    enable_row=row("enabled","Display panel test",AZ_ROW_TOGGLE,AZ_ROW_DEFAULT_ON,"Read-only SDK test. No audio effects.",NULL,0);
    dim_row=row("dim-indicators","Dim Vocals/Instrument",AZ_ROW_TOGGLE,AZ_ROW_DEFAULT_ON,"Dim indicators when the selected deck STEMS engine is off. No per-stem mute feedback yet.",NULL,0);
    follow_row=row("follow","Follow mixer (test)",AZ_ROW_TOGGLE,0,"Set all four assignments below to match Utility. Uses faders, not audio detection.",NULL,0);
    static const char *const choices[]={"Unknown","A","THRU","B"};
    static const char *const keys[]={"assign-1","assign-2","assign-3","assign-4"};
    static const char *const labels[]={"CH 1 crossfader","CH 2 crossfader","CH 3 crossfader","CH 4 crossfader"};
    for(int d=0;d<4;d++) assign_row[d]=row(keys[d],labels[d],AZ_ROW_CHOICE,0,"Match the player's Utility crossfader assignment.",choices,4);
    if(enable_row<0 || dim_row<0 || follow_row<0) return 2;
    for(int d=0;d<4;d++) if(assign_row[d]<0) return 2;
    int saved=C->cfg_get(ID,"deck",-1);selected=saved>=0&&saved<4?saved:-1;
    button_surface=C->surface_create(ID,&BUTTON);panel_surface=C->surface_create(ID,&PANEL);
    if(!button_surface || !panel_surface) return 3;
    C->surface_place(button_surface,(az_rect){1090,8,182,48});
    C->surface_place(panel_surface,(az_rect){48,360,1184,400});
    if(C->frame_filter(ID,AZ_FILTER_RAW|(1u<<AZ_BUS_MIXER_IN),frame,NULL)) return 4;
    return 0;
}
static const char *const FW[]={"any",NULL};
static const az_mod_v1 MOD={.abi=AZ_MOD_ABI,.size=sizeof(az_mod_v1),.id=ID,.fw_tags=FW,.init=init,.tick=tick};
const az_mod_v1 *az_mod_v1_entry(void) {return &MOD;}
