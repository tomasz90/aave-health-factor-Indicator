void updateChain(char* chain) {
  for (int i = 0; i < sizeof(settings.chain); i++) {
    settings.chain[i] = chain[i];
  }
  EEPROM.put(0, settings);
  EEPROM.commit();
}

void updateAddress(char* address) {
  for (int i = 0; i < sizeof(settings.address); i++) {
    settings.address[i] = address[i];
  }
  EEPROM.put(0, settings);
  EEPROM.commit();
}
