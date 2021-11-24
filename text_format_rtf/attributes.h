#pragma once


namespace text_format_rtf
{


   class attributes
   {
   public:


      virtual ~attributes() { }

      virtual void enable_attr(e_attr eattr, int iParam, bool bEnable);

      virtual void attr_push(e_attr eattr, int iParam) = 0;
      virtual void attrstack_push() = 0;
      virtual void attrstack_drop() = 0;
      virtual int attr_pop(e_attr eattr) = 0;
      virtual void attr_drop_all() = 0;
      virtual void attr_pop_all() = 0;
      virtual void attr_pop_dump() = 0;
      virtual void attr_remove(e_attr * peattr, int size) = 0;
      virtual void attr_push_style(e_element type, int id) = 0;

      virtual void register_style(const style & style) = 0;

   };


} // namespace text_format_rtf



