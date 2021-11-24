//   LibRTF - SourceForge.net  - Original Author 2006 by Tomasz Hławiczka (C)                               
//   ca2-ing by Camilo Sasuke 2019-07-22
//
#include "framework.h"
#include "_text_format_rtf.h"


namespace text_format_rtf
{

   void attributes::enable_attr(enum text_format_rtf::e_attr, int, bool)
   {

   }

   void attributes_stack::enable_attr(enum text_format_rtf::e_attr, int, bool)
   {

   }


   void attributes_stack::attr_push_style(e_element eelement, int style_id)
   {

      style styleKey;

      styleKey.id = style_id;

      styleKey.m_eelement = eelement;

      auto iFind = m_stylea.find(styleKey);

      if (iFind == m_stylea.end())
      {
         return;

      }

      auto & ref = *iFind;

      if (ref.bold == 1)
         attr_push(attr_bold, 0);
      else if (ref.bold == 0)
         attr_pop(attr_bold);

      if (ref.italic == 1)
         attr_push(attr_italic, 0);
      else if (ref.italic == 0)
         attr_pop(attr_italic);

      if (ref.fontface >= 0)
         attr_push(attr_fontface, ref.fontface);

      if (ref.fontsize >= 0)
         attr_push(attr_fontsize, ref.fontsize);

   }


   void attributes_stack::register_style(const style & style)
   {

      m_stylea.insert(style);

   }


   void attributes_stack::attr_push(e_attr eattr, int iParam)
   {

      if (m_attributestack.empty())
      {

         return;

      }

      m_attributestack.back().push_back(attribute(eattr, iParam));

      enable_attr(eattr, iParam, true);

   }


   void attributes_stack::attrstack_push()
   {

      m_attributestack.push_back(block());

   }

   int attributes_stack::get_attr(e_attr eattr)
   {

      for (auto block = m_attributestack.rbegin(); block != m_attributestack.rend(); block++)
      {

         for (auto attr = block->rbegin(); attr != block->rend(); attr++)
         {

            if (attr->m_eattr == eattr)
            {

               return attr->m_iParam;

            }
      
         }

      }

      return-1;

   }


   void attributes_stack::attrstack_drop()
   {

      if (m_attributestack.empty())
      {

         return;

      }

      block & b = m_attributestack.back();

      while (!b.empty())
      {

         attribute & attr = b.back();

         enable_attr(attr.m_eattr, attr.m_iParam, false);

         b.pop_back();

      }

      m_attributestack.pop_back();

   }


   int attributes_stack::attr_pop(e_attr eattr)
   {

      if (m_attributestack.empty())
      {

         return 0;

      }

      block & blk = m_attributestack.back();

      if (blk.empty())
      {

         return 0;

      }

      if (blk.back().m_eattr == eattr)
      {

         enable_attr(blk.back().m_eattr, blk.back().m_iParam, false);

         blk.pop_back();

      }
      else
      {

         ::std::list < attribute > backup;

         while(!blk.empty())
         {

            if(blk.back().m_eattr == eattr)
            {

               enable_attr(blk.back().m_eattr,blk.back().m_iParam,false);

               blk.pop_back();

               break;

            }
            else
            {

               backup.push_front(blk.back());

               blk.pop_back();

            }

         }

         for(auto it = backup.begin(),et = backup.end(); it != et; ++it)
         {

            blk.push_back(*it);

            enable_attr((*it).m_eattr,(*it).m_iParam,true);

         }

      }

      return 1;

   }


   void attributes_stack::attr_drop_all()
   {

      if (m_attributestack.empty())
      {

         return;

      }

      m_attributestack.pop_back();

   }


   void attributes_stack::attr_pop_all()
   {

      if (m_attributestack.empty())
      {

         return;

      }

      block & b = m_attributestack.back();

      while (!b.empty())
      {

         attribute & attr = b.back();

         enable_attr(attr.m_eattr, attr.m_iParam, false);

         b.pop_back();

      }

   }


   void attributes_stack::attr_pop_dump()
   {

      if (m_attributestack.empty())
      {

         return;

      }

      block & b = m_attributestack.back();

      while(!b.empty())
      {

         attribute & attr = b.back();

         enable_attr(attr.m_eattr, attr.m_iParam, false);

         b.pop_back(); // FIXME this version should not remove m_attributestack from stack.

      }

   }


   void attributes_stack::attr_remove(e_attr * peattr, int size)
   {

      if (m_attributestack.empty())
      {

         return;

      }

      block & b = m_attributestack.back();

      attribute & attr = b.back();

      for (int i = size; i--; )
      {

         if (attr.m_eattr == peattr[i])
         {

            enable_attr(attr.m_eattr, attr.m_iParam, false);

            b.pop_back();

            break; // FIXME not only last agrument should be checked, another container is needed.

         }

      }

   }


} // namespace text_format_rtf



