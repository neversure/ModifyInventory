Программа считает модифицированные характеристика инвентаря.

На вход получает 4 строчки вида (оружие)
{ident: "axe_01", type: Melee, level: 3, rarity: Common, damage: 12.0, speed: 0.9 },
и две строчки вида (броня)
{ident: "jacket_01", type: Armour, level: 2, rarity: Common, protection: 2.0 },
а так же еще три строчки вида (модификаторы)
{ ident: "donatium_steel", filters: [{"rarity", Rare}], type: ProtectionBuff, value: 15 },
Филтры модификаторов (filters: [] ) - условия, при соответсвии которым предмет получит прибавку. Их может быть несколько.

Остальные входные данные в файле input.txt

