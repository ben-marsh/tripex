#pragma once
#include "CCfgItem.h"

void CreateCfgItems();
void LoadCfgItems();
void SaveCfgItems();
void UpdateCfgItems(bool bInit = false);
CCfgItem *FindCfgItem(const char *sName);
