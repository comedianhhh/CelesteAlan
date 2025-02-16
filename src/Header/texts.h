enum Localization
{
  LOCALIZATION_ENG,
  //TODO

  LOCALIZATION_COUNT
};

enum StringID
{
  STRING_CELESTE_CLONE,
  STRING_MADE_IN_CPP,
  STRING_THIS_IS_ALANS_WORLD,
  STRING_COUNT
};

const char* Strings[(int)STRING_COUNT * (int)LOCALIZATION_COUNT];
static Localization localization = LOCALIZATION_ENG;

void init_strings()
{
  Strings[(int)LOCALIZATION_ENG + (int)STRING_CELESTE_CLONE] = "Celeste Clone";
  Strings[(int)LOCALIZATION_ENG + (int)STRING_MADE_IN_CPP] = "Made in C++";
  Strings[(int)LOCALIZATION_ENG + (int)STRING_THIS_IS_ALANS_WORLD] = "This is ALan's World\nYou can create your own levels\n****";

  //TODO
  

}

// This is the get string function

const char* _(StringID stringID)
{
  return Strings[(int)localization * (int)STRING_COUNT + (int)stringID];
}