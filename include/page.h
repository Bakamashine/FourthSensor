#ifndef PAGE_H
#define PAGE_H

typedef enum Pages
{
  MAIN,
  SETTINGS,
  ERROR,
  COUNT_PAGES
} Pages;

extern int currentPage;
#endif