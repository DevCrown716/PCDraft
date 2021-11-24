#pragma once


namespace text_format_rtf
{


   class attributes_stack 
   {
   public:

      typedef ::std::vector < attribute > block;
      typedef ::std::vector < block > attribute_stack;
      typedef ::std::set < style > styles_array;


      styles_array            m_stylea;
      attribute_stack         m_attributestack;

      
      virtual void enable_attr(e_attr eattr, int iParam, bool bEnable);


      virtual void attr_push(e_attr eattr, int iParam);
      virtual void attrstack_push();
      virtual void attrstack_drop();
      virtual int attr_pop(e_attr eattr);
      virtual void attr_drop_all();
      virtual void attr_pop_all();
      virtual void attr_pop_dump();
      virtual void attr_remove(e_attr * peattr, int size);

      virtual void attr_push_style(e_element type, int style_id);

      virtual void register_style(const style & style);


      virtual int get_attr(e_attr eattr);


   };


} // namespace text_format_rtf



