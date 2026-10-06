#ifndef PAGE_H
#define PAGE_H

typedef enum Pages
{
  MAIN,
  SETTINGS,
  COUNT_PAGES
} Pages;

static int currentPage = MAIN;
#endif