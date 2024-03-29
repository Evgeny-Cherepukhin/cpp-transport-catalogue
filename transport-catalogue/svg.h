// Черепухин Евгений Сергеевич. Спринт 14 Версия 1. 
#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>


namespace svg {

	enum class StrokeLineCap {
		BUTT,
		ROUND,
		SQUARE,
	};

	enum class StrokeLineJoin {
		ARCS,
		BEVEL,
		MITER,
		MITER_CLIP,
		ROUND,
	};

	struct Rgb {
		Rgb() = default;
		Rgb(uint8_t red, uint8_t green, uint8_t blue)
			:red(red), green(green), blue(blue) {
		}
		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
	};

	inline std::ostream& operator<<(std::ostream& out, Rgb color);

	struct Rgba {
		Rgba() = default;
		Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) :red(red), green(green), blue(blue), opacity(opacity) {}
		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
		double opacity = 1.0;
	};

	inline std::ostream& operator<<(std::ostream& out, Rgba color);

	using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

	std::ostream& operator<<(std::ostream& out, Color color);

	uint8_t Lerp(uint8_t from, uint8_t to, double t);

	svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t);




	std::ostream& operator<<(std::ostream& out, const StrokeLineCap cap);

	std::ostream& operator<<(std::ostream& out, const StrokeLineJoin join);

	// Объявив в заголовочном файле константу со спецификатором inline,
	// мы сделаем так, что она будет одной на все единицы трансляции,
	// которые подключают этот заголовок.
	// В противном случае каждая единица трансляции будет использовать свою копию этой константы.
	inline const Color NoneColor{ "none" };


	template <typename Owner>
	class PathProps {
	public:
		Owner& SetFillColor(Color color) {
			fill_color_ = std::move(color);
			return AsOwner();
		}

		Owner& SetStrokeColor(Color color) {
			stroke_color_ = std::move(color);
			return AsOwner();
		}

		Owner& SetStrokeWidth(double width) {
			stroke_width_ = width;
			return AsOwner();
		}

		Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
			line_cap_ = line_cap;
			return AsOwner();
		}

		Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
			stroke_line_join_ = line_join;
			return AsOwner();
		}
	protected:
		~PathProps() = default;

		void RenderAttrs(std::ostream& out) const {
			using namespace std::literals;

			if (fill_color_) {
				out << " fill=\""sv << *fill_color_ << "\""sv;
			}
			if (stroke_color_) {
				out << " stroke=\""sv << *stroke_color_ << "\""sv;
			}
			if (stroke_width_) {
				out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
			}
			if (line_cap_) {
				out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
			}
			if (stroke_line_join_) {
				out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
			}
		}

	private:
		Owner& AsOwner() {
			return static_cast<Owner&>(*this);
		}

		std::optional<double> stroke_width_;
		std::optional<StrokeLineCap> line_cap_;
		std::optional<StrokeLineJoin> stroke_line_join_;
		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
	};
	struct Point {
		Point() = default;
		Point(double x, double y)
			: x(x)
			, y(y) {
		}
		double x = 0;
		double y = 0;
	};

	/*
	 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
	 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
	 */
	struct RenderContext {
		RenderContext(std::ostream& out)
			: out(out) {
		}

		RenderContext(std::ostream& out, int indent_step, int indent = 0)
			: out(out)
			, indent_step(indent_step)
			, indent(indent) {
		}

		RenderContext Indented() const;

		void RenderIndent() const;

		std::ostream& out;
		int indent_step = 0;
		int indent = 0;
	};

	/*
	 * Абстрактный базовый класс Object служит для унифицированного хранения
	 * конкретных тегов SVG-документа
	 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
	 */
	class Object {
	public:
		void Render(const RenderContext& context) const;

		virtual ~Object() = default;

	private:
		virtual void RenderObject(const RenderContext& context) const = 0;
	};

	/*
	 * Класс Circle моделирует элемент <circle> для отображения круга
	 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
	 */
	class Circle final : public Object, public PathProps<Circle> {
	public:
		Circle& SetCenter(Point center);
		Circle& SetRadius(double radius);

	private:
		void RenderObject(const RenderContext& context) const override;

		Point center_;
		double radius_ = 1.0;
	};

	/*
	 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
	 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
	 */
	class Polyline final : public Object, public PathProps<Polyline> {
	public:
		// Добавляет очередную вершину к ломаной линии
		Polyline& AddPoint(Point point);

		/*
		 * Прочие методы и данные, необходимые для реализации элемента <polyline>
		 */
	private:
		void RenderObject(const RenderContext& context) const override;

		std::vector<Point> points_;
	};

	/*
	 * Класс Text моделирует элемент <text> для отображения текста
	 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
	 */
	class Text final : public Object, public PathProps<Text> {
	public:
		// Задаёт координаты опорной точки (атрибуты x и y)
		Text& SetPosition(Point pos);

		// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
		Text& SetOffset(Point offset);

		// Задаёт размеры шрифта (атрибут font-size)
		Text& SetFontSize(uint32_t size);

		// Задаёт название шрифта (атрибут font-family)
		Text& SetFontFamily(std::string font_family);

		// Задаёт толщину шрифта (атрибут font-weight)
		Text& SetFontWeight(std::string font_weight);

		// Задаёт текстовое содержимое объекта (отображается внутри тега text)
		Text& SetData(std::string data);

		// Прочие данные и методы, необходимые для реализации элемента <text>
	private:
		void RenderObject(const RenderContext& context) const override;

		uint32_t size_ = 1;
		Point position_ = { 0,0 };
		Point offset_ = { 0,0 };
		std::string font_family_;
		std::string font_weight_;
		std::string value_;
	};

	class ObjectContainer {
		/*
		 Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
		 Пример использования:
		 Document doc;
		 doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
		*/
	public:
		template <typename Obj>
		void Add(Obj obj) {
			AddPtr(std::make_unique<Obj>(std::move(obj)));
		}

		virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

		virtual ~ObjectContainer() = default;
	};

	class Drawable {
	public:
		virtual void Draw(ObjectContainer&) const = 0;

		virtual ~Drawable() = default;
	};

	class Document : public ObjectContainer {
	public:
		// Добавляет в svg-документ объект-наследник svg::Object
		void AddPtr(std::unique_ptr<Object>&& obj);

		// Выводит в ostream svg-представление документа
		void Render(std::ostream& out) const;

		// Прочие методы и данные, необходимые для реализации класса Document

	private:
		std::vector<std::shared_ptr<Object>> objects_;
	};

}  // namespace svg