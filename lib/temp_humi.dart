// ignore_for_file: public_member_api_docs, sort_constructors_first

class TempHumi {
  final double temp;
  final double humi;

  TempHumi({required this.temp, required this.humi});

  factory TempHumi.fromJson(Map<String, dynamic> json) {
    return TempHumi(
      temp: (json["temperature"] as num).toDouble(), // Chuyển thành double
      humi: (json["humidity"] as num).toDouble(), // Chuyển thành double
    );
  }
}
