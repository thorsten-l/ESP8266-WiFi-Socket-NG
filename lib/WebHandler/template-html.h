// file name = info.html, length = 2183
const char TEMPLATE_HEADER[] =
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<title>WiFi Socket NG</title>"
  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
  "<link rel=\"stylesheet\" href=\"pure-min.css\">"
  "<link rel=\"stylesheet\" href=\"layout.css\">"
  "</head>"
  "<body>"
  "<div class=\"custom-menu-wrapper\">"
  "<div class=\"pure-menu custom-menu custom-menu-top\">"
  "<div id=\"brand-title\" class=\"pure-menu-heading custom-menu-brand\">%s</div>"
  "<a href=\"#\" class=\"custom-menu-toggle\" id=\"toggle\"><s class=\"bar\"></s><s class=\"bar\"></s></a>"
  "</div>"
  "<div class=\"pure-menu pure-menu-horizontal pure-menu-scrollable custom-menu custom-menu-bottom custom-menu-tucked\" id=\"tuckedMenu\">"
  "<div class=\"custom-menu-screen\"></div>"
  "<ul class=\"pure-menu-list\">"
  "<li class=\"pure-menu-item\"><a href=\"/\" class=\"pure-menu-link\">Home</a></li>"
  "<li class=\"pure-menu-item\"><a href=\"setup.html\" class=\"pure-menu-link\">Setup</a></li>"
  "<li class=\"pure-menu-item\"><a href=\"info.html\" class=\"pure-menu-link\">Info</a></li>"
  "</ul>"
  "</div>"
  "</div>"
  "<div class=\"main\">"
  "<div class=\"content\">";

const char TEMPLATE_FOOTER[] =
  "</div>"
  "</div>"
  "<script>"
  "(function (window, document) {"
  "document.getElementById('toggle').addEventListener('click', function (e) {"
  "document.getElementById('tuckedMenu').classList.toggle('custom-menu-tucked');"
  "document.getElementById('toggle').classList.toggle('x');"
  "});"
  "})(this, this.document);"
  "</script>"
  "</body>"
  "</html>";
