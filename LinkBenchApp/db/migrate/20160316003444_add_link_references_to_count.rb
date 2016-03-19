class AddLinkReferencesToCount < ActiveRecord::Migration
  def change
    add_reference :counts, :link, index: true, foreign_key: true
  end
end
