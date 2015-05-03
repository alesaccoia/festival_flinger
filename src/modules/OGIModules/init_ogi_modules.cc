#include "EST_unix.h"
#include "stdio.h"
#include "festival.h"

void festival_OGIdbase_init(void);
void festival_OGIcommon_init(void);
void festival_OGIeffect_init(void);
void festival_OGIinsert_init(void);
void festival_OGIresLPC_init(void);
void festival_Flinger_init(void);

void festival_OGIModules_init(void)
{
  festival_OGIdbase_init();
  festival_OGIcommon_init();
  festival_OGIeffect_init();
  festival_OGIinsert_init();
  festival_OGIresLPC_init();
  festival_Flinger_init();
}
