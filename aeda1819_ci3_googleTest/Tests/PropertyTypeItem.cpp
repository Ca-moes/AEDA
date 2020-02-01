#include "PropertyTypeItem.h"

PropertyTypeItem::PropertyTypeItem(string address, string postalCode, string typology, int maxPrice) {
	this->address = address;
	this->postalCode = postalCode;
	this->typology = typology;
	this->maxPrice = maxPrice;
}

string PropertyTypeItem::getAddress() const {
	return this->address;
}

string PropertyTypeItem::getPostalCode() const {
	return this->postalCode;
}

string PropertyTypeItem::getTypology() const {
	return this->typology;
}

int PropertyTypeItem::getMaxPrice() const {
	return this->maxPrice;
}

vector<Property*> PropertyTypeItem::getItems() const {
	return this->items;
}

void PropertyTypeItem::setItems(vector<Property*> properties) {
	items = properties;
}

void PropertyTypeItem::addItems(Property* property) {
	this->items.push_back(property);
}

//
// TODO
//

bool PropertyTypeItem::operator<(const PropertyTypeItem &pti1) const {
    //TODO:
    if (address < pti1.address){
        return true;
    } else if (address == pti1.address){
        if (postalCode < pti1.postalCode){
            return true;
        } else if (postalCode == pti1.postalCode){  // pelo enunciado 2 mesmas moradas têm postalcodde diferentes, a partir daqui é so precaução
            if (typology < pti1.typology)
                return true;
            else if (typology == pti1.typology){
                if (maxPrice < pti1.maxPrice)
                    return true;
            }
        }
    }
	return false;
}

bool PropertyTypeItem::operator==(const PropertyTypeItem &pti1) const {
    //TODO:
    if (address == pti1.address){
        if (postalCode == pti1.postalCode){
            if (typology == pti1.typology){
                if (maxPrice == pti1.maxPrice){
                    if(items == pti1.items)
                        return true;
                }
            }
        }
    }
	return false;
}
