
class AutoDestination {
  final String description;
  final String name;
  final String icon;

  const AutoDestination({
    required this.description,
    required this.name,
    required this.icon,
  });

  factory AutoDestination.fromJson(Map<String, dynamic> json) {
    return AutoDestination(
      description: json['description'],
      name: json['name'],
      icon: json['icon'],
    );
  }
}

class AutoDestinations {
  final List<AutoDestination> autoDestinations;

  const AutoDestinations({required this.autoDestinations});

  factory AutoDestinations.fromJson(dynamic json) {
    var result = json
        .map<AutoDestination>((json) => AutoDestination.fromJson(json))
        .toList();
    return AutoDestinations(autoDestinations: result);
  }
}

class AutomationElement {
  final String description;
  final String id;
  final String typeName;
  final String setParameter;
  final String valueParameter;
  final String value;
  

  AutomationElement(
      {required this.description, required this.id, required this.typeName, required this.setParameter, required this.valueParameter, required this.value});

  factory AutomationElement.fromJson(Map<String, dynamic> json) {
    return AutomationElement(
        description: json['description'],
        id: json['id'],
        typeName: json['typeName'],
        setParameter: json['setParameter'],
        valueParameter: json['valueParameter'],
        value: json['value']);
  }
}

class AutomationPageConfig {
  final String name;
  final List<AutomationElement> elements;

  AutomationPageConfig({required this.name, required this.elements});

  factory AutomationPageConfig.fromJson(Map<String, dynamic> json) {
    var elementsIntern = json['elements']
        .map<AutomationElement>((json) => AutomationElement.fromJson(json))
        .toList();
    /*List<AutomationElement> elementsIntern = List.empty();
    for (var entry in elementsJson.automationElement) {
      elementsIntern.add(entry);
    }*/

    return AutomationPageConfig(name: json['name'], elements: elementsIntern);
  }
}